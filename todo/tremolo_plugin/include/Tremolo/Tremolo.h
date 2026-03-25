#pragma once

namespace tremolo
{
    class Tremolo
    {
        public:
            enum class LfoWaveform : size_t{
                sine = 0,
                triangle = 1,
                sawtooth = 2,
            };
          Tremolo()
          {
            for (auto& lfo : lfos)
            {
              lfo.setFrequency(5.f /* Hz */, true);
            }
          }
          void prepare(double sampleRate, int expectedMaxFramesPerBlock)
          {
            const juce::dsp::ProcessSpec processSpec
            {
            .sampleRate = sampleRate,
            .maximumBlockSize = static_cast<juce::uint32>(expectedMaxFramesPerBlock),
            .numChannels = 1u,
            };


            lfoMix.reset(64);

            smoothedGainParameter.reset(sampleRate, 0.01);

            for (auto& lfo : lfos)
            {
              lfo.prepare(processSpec);
            }
          }

          void setLfoWaveform(LfoWaveform waveform)
          {
            jassert(waveform == LfoWaveform::sine || waveform == LfoWaveform::sawtooth || waveform == LfoWaveform::triangle);

            lfoToSet = waveform;
          }

          void setModulationRate(float rateHz) {
              for (auto& lfo : lfos) {
                  lfo.setFrequency(rateHz);
              }
          }

          void setOutputGain(float gainDb) {
            const auto linearGain = juce::Decibels::decibelsToGain(gainDb);

                smoothedGainParameter.setTargetValue(linearGain);
          }


          void process(juce::AudioBuffer<float>& buffer) noexcept
          {
            updateLfoWaveform();
            // for each frame
            for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples()))
            {
              // generate the LFO value
              const auto lfoValue = getNextLfoValue();

              const auto gainSetting = smoothedGainParameter.getNextValue();

                // calculate the modulation value
              constexpr auto modulationDepth = 0.4f;
              const auto modulationValue = modulationDepth * (lfoValue + 1.f);



              // for each channel sample in the frame
              for (const auto channelIndex :
                   std::views::iota(0, buffer.getNumChannels()))
              {
                // get the input sample
                const auto inputSample = buffer.getSample(channelIndex, frameIndex);

                // modulate the sample
                const auto outputSample = inputSample * modulationValue * gainSetting;

                // set the output sample
                buffer.setSample(channelIndex, frameIndex, outputSample);
              }
            }
          }



            void reset() noexcept
            {
                for (auto& lfo : lfos)
                    {
                      lfo.reset();
                    }
            }
        private:
          // You should put class members and private functions here

            juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> lfoMix;

            juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedGainParameter;

            static float triangle(float phase)
            {
                return std::abs(2 * phase / juce::MathConstants<float>::pi) - 1.f;
            }

            static float sawtooth(float phase)
            {
                float p = phase / juce::MathConstants<float>::pi;
                return 2 * std::abs(p - 1.f) - 1.f;
            }

            //update to include smoothing
            float getNextLfoValue()
            {
                    if
                        (lfoMix.isSmoothing())
                    {
                        return lfoMix.getNextValue();
                    }

                return lfos[juce::toUnderlyingType(currentLfo)].processSample(0.f);
            }

            //update to include smoothing
            void updateLfoWaveform()
            {
                if (currentLfo != lfoToSet)
                {
                    const float oldSample{lfos[juce::toUnderlyingType(currentLfo)].processSample(0.f)};

                    currentLfo = lfoToSet;
                    const float newSample{lfos[juce::toUnderlyingType(currentLfo)].processSample(0.f)};
                    lfoMix.setCurrentAndTargetValue(oldSample);
                    lfoMix.setTargetValue(newSample);
                }
            }

          //juce::dsp::Oscillator<float> lfo{ [](auto phase){ return std::sin(phase); }};

          std::array<juce::dsp::Oscillator<float>, 3u> lfos
          {

                  juce::dsp::Oscillator<float>{[](auto phase) { return std::sin(phase); }},
                  juce::dsp::Oscillator<float>{triangle},
                  juce::dsp::Oscillator<float>{sawtooth},
          };


          LfoWaveform currentLfo = LfoWaveform::sine;
          LfoWaveform lfoToSet = currentLfo;
    };
}  // namespace tremolo
