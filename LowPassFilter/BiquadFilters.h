#pragma once
#include <cmath>
#include <stdexcept>

constexpr double M_PI = 3.14159265358979323846;

/**
 * @brief Enumeration for supported biquad filter types.
 */
enum class BiquadFilterType {
    LowPass,
    HighPass,
    BandPass, // Constant Q, peak gain = Q
    Notch,
    Peak,
    LowShelf,
    HighShelf
};

/**
 * @brief A second-order biquad filter class for sample-by-sample processing.
 *
 * Implements Direct Form II Transposed for numerical stability.
 * Coefficients are calculated based on standard cookbook formulas.
 */
class BiquadFilter {
public:
    /**
     * @brief Constructor for the BiquadFilter.
     * @param sampleRate The audio sample rate in Hz.
     */
    BiquadFilter(float sampleRate) :
        sampleRate_(sampleRate),
        b0_(0.0f), b1_(0.0f), b2_(0.0f),
        a1_(0.0f), a2_(0.0f),
        s1_(0.0f), s2_(0.0f)
    {
        // Default to a passthrough filter (all coefficients zero except b0=1)
        setPassthrough();
    }

    /**
     * @brief Processes a single input sample through the filter.
     * @param input_sample The input audio sample (float).
     * @return The filtered output sample (float).
     */
    float processSample(float input_sample) {
        // Direct Form II Transposed implementation
        float output_sample = b0_ * input_sample + s1_;
        s1_ = b1_ * input_sample - a1_ * output_sample + s2_;
        s2_ = b2_ * input_sample - a2_ * output_sample;
        return output_sample;
    }

    /**
     * @brief Resets the internal state of the filter.
     * Call this when starting a new audio stream or after a discontinuity.
     */
    void reset() {
        s1_ = 0.0f;
        s2_ = 0.0f;
    }

    /**
     * @brief Sets the filter to act as a simple passthrough (no filtering).
     * Resets internal state and sets coefficients for a flat response.
     */
    void setPassthrough() {
        b0_ = 1.0f; b1_ = 0.0f; b2_ = 0.0f;
        a1_ = 0.0f; a2_ = 0.0f;
        reset();
    }

    /**
     * @brief Sets the sample rate for the filter.
     * @param sampleRate The new sample rate in Hz.
     * @warning Changing sample rate requires recalculating coefficients.
     * Call a filter type setter (e.g., setLowPass) after this.
     */
    void setSampleRate(float sampleRate) {
        if (sampleRate <= 0) {
            throw std::invalid_argument("Sample rate must be positive.");
        }
        sampleRate_ = sampleRate;
        // Coefficients become invalid, user should call a setFilterType method
    }

    /**
     * @brief Sets up a Low Pass filter.
     * @param cutoff_freq The cutoff frequency in Hz.
     * @param Q The Q factor (controls resonance; typically 0.707 for Butterworth).
     */
    void setLowPass(float cutoff_freq, float Q) {
        calculateCoefficients(BiquadFilterType::LowPass, cutoff_freq, Q, 0.0f);
    }

    /**
     * @brief Sets up a High Pass filter.
     * @param cutoff_freq The cutoff frequency in Hz.
     * @param Q The Q factor (controls resonance; typically 0.707 for Butterworth).
     */
    void setHighPass(float cutoff_freq, float Q) {
        calculateCoefficients(BiquadFilterType::HighPass, cutoff_freq, Q, 0.0f);
    }

    /**
     * @brief Sets up a Band Pass filter (constant Q type).
     * @param center_freq The center frequency in Hz.
     * @param Q The Q factor.
     */
    void setBandPass(float center_freq, float Q) {
        calculateCoefficients(BiquadFilterType::BandPass, center_freq, Q, 0.0f);
    }

    /**
     * @brief Sets up a Notch filter.
     * @param center_freq The center frequency in Hz.
     * @param Q The Q factor (controls notch width).
     */
    void setNotch(float center_freq, float Q) {
        calculateCoefficients(BiquadFilterType::Notch, center_freq, Q, 0.0f);
    }

    /**
     * @brief Sets up a Peak (Bell) filter.
     * @param center_freq The center frequency in Hz.
     * @param Q The Q factor.
     * @param dBgain The gain in dB (positive for boost, negative for cut).
     */
    void setPeak(float center_freq, float Q, float dBgain) {
        calculateCoefficients(BiquadFilterType::Peak, center_freq, Q, dBgain);
    }

    /**
     * @brief Sets up a Low Shelf filter.
     * @param cutoff_freq The cutoff frequency in Hz.
     * @param S The shelf slope parameter (e.g., 1.0 for 6dB/octave).
     * @param dBgain The gain in dB (positive for boost, negative for cut).
     */
    void setLowShelf(float cutoff_freq, float S, float dBgain) {
        calculateCoefficients(BiquadFilterType::LowShelf, cutoff_freq, S, dBgain);
    }

    /**
     * @brief Sets up a High Shelf filter.
     * @param cutoff_freq The cutoff frequency in Hz.
     * @param S The shelf slope parameter (e.g., 1.0 for 6dB/octave).
     * @param dBgain The gain in dB (positive for boost, negative for cut).
     */
    void setHighShelf(float cutoff_freq, float S, float dBgain) {
        calculateCoefficients(BiquadFilterType::HighShelf, cutoff_freq, S, dBgain);
    }

private:
    float sampleRate_;
    float b0_, b1_, b2_; // Numerator coefficients
    float a1_, a2_;     // Denominator coefficients (a0 is normalized to 1)
    float s1_, s2_;     // Internal state variables (Direct Form II Transposed)

    /**
     * @brief Calculates filter coefficients based on type and parameters.
     * @param type The type of biquad filter.
     * @param freq The cutoff or center frequency in Hz.
     * @param Q_or_S The Q factor for resonant filters, or S for shelf filters.
     * @param dBgain The gain in dB for peak/shelf filters.
     */
    void calculateCoefficients(BiquadFilterType type, float freq, float Q_or_S, float dBgain) {
        if (freq <= 0 || freq >= sampleRate_ / 2.0f) {
            throw std::invalid_argument("Frequency must be greater than 0 and less than Nyquist frequency (sampleRate / 2).");
        }
        if (Q_or_S <= 0 && type != BiquadFilterType::LowShelf && type != BiquadFilterType::HighShelf) {
            throw std::invalid_argument("Q factor must be positive.");
        }

        // Intermediate variables from RBJ Cookbook
        float omega = 2.0f * M_PI * freq / sampleRate_;
        float sn = std::sin(omega);
        float cs = std::cos(omega);
        float alpha = sn / (2.0f * Q_or_S);
        float A = std::pow(10.0f, dBgain / 40.0f); // For Peak/Shelf only, convert dB to linear gain

        float a0_val = 1.0f; // This will be the true a0, for normalization

        // Initialize numerator (b) and denominator (a) coefficients
        float b0_temp = 0.0f, b1_temp = 0.0f, b2_temp = 0.0f;
        float a1_temp = 0.0f, a2_temp = 0.0f;

        switch (type) {
        case BiquadFilterType::LowPass:
            b0_temp = (1.0f - cs) / 2.0f;
            b1_temp = 1.0f - cs;
            b2_temp = (1.0f - cs) / 2.0f;
            a0_val = 1.0f + alpha;
            a1_temp = -2.0f * cs;
            a2_temp = 1.0f - alpha;
            break;

        case BiquadFilterType::HighPass:
            b0_temp = (1.0f + cs) / 2.0f;
            b1_temp = -(1.0f + cs);
            b2_temp = (1.0f + cs) / 2.0f;
            a0_val = 1.0f + alpha;
            a1_temp = -2.0f * cs;
            a2_temp = 1.0f - alpha;
            break;

        case BiquadFilterType::BandPass: // Constant Q, peak gain = Q
            b0_temp = alpha;
            b1_temp = 0.0f;
            b2_temp = -alpha;
            a0_val = 1.0f + alpha;
            a1_temp = -2.0f * cs;
            a2_temp = 1.0f - alpha;
            break;

        case BiquadFilterType::Notch:
            b0_temp = 1.0f;
            b1_temp = -2.0f * cs;
            b2_temp = 1.0f;
            a0_val = 1.0f + alpha;
            a1_temp = -2.0f * cs;
            a2_temp = 1.0f - alpha;
            break;

        case BiquadFilterType::Peak:
            b0_temp = 1.0f + alpha * A;
            b1_temp = -2.0f * cs;
            b2_temp = 1.0f - alpha * A;
            a0_val = 1.0f + alpha / A;
            a1_temp = -2.0f * cs;
            a2_temp = 1.0f - alpha / A;
            break;

        case BiquadFilterType::LowShelf: {
            // Shelf slope S is typically 1.0 (for 6dB/octave)
            float beta = std::sqrt(A) / Q_or_S; // Q_or_S is S for shelf filters
            b0_temp = A * ((A + 1.0f) - (A - 1.0f) * cs + beta * sn);
            b1_temp = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cs);
            b2_temp = A * ((A + 1.0f) - (A - 1.0f) * cs - beta * sn);
            a0_val = (A + 1.0f) + (A - 1.0f) * cs + beta * sn;
            a1_temp = -2.0f * ((A - 1.0f) + (A + 1.0f) * cs);
            a2_temp = (A + 1.0f) + (A - 1.0f) * cs - beta * sn;
            break;
        }

        case BiquadFilterType::HighShelf: {
            // Shelf slope S is typically 1.0 (for 6dB/octave)
            float beta = std::sqrt(A) / Q_or_S; // Q_or_S is S for shelf filters
            b0_temp = A * ((A + 1.0f) + (A - 1.0f) * cs + beta * sn);
            b1_temp = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cs);
            b2_temp = A * ((A + 1.0f) + (A - 1.0f) * cs - beta * sn);
            a0_val = (A + 1.0f) - (A - 1.0f) * cs + beta * sn;
            a1_temp = 2.0f * ((A - 1.0f) - (A + 1.0f) * cs);
            a2_temp = (A + 1.0f) - (A - 1.0f) * cs - beta * sn;
            break;
        }
        }

        // Normalize coefficients by dividing by a0_val
        b0_ = b0_temp / a0_val;
        b1_ = b1_temp / a0_val;
        b2_ = b2_temp / a0_val;
        a1_ = a1_temp / a0_val;
        a2_ = a2_temp / a0_val;

        // Reset state after coefficient change to prevent clicks
        reset();
    }
};