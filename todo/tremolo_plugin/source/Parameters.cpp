
namespace tremolo {
namespace {
    auto& addParameterToProcessor(juce::AudioProcessor& processor, auto parameter){
        auto& parameterReference = *parameter;
        processor.addParameter(parameter.release());
        return parameterReference;
    }
  juce::AudioParameterFloat &createModulationRateParameter(juce::AudioProcessor &processor) {
    constexpr auto versionHint = 1;
    auto parameter = std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"modulation.rate", versionHint}
            , "Modulation Rate",
            juce::NormalisableRange{0.1f, 20.f, 0.01f, 0.4f}
            , 5.f,
            juce::AudioParameterFloatAttributes{}.withLabel("Hz"));
    return addParameterToProcessor(processor, std::move(parameter));
  }

  juce::AudioParameterFloat &createGainParameter(juce::AudioProcessor &processor){
      constexpr auto versionHint = 1;
      auto parameter = std::make_unique<juce::AudioParameterFloat>(
              juce::ParameterID{"gain.control", versionHint}
              ,"Gain",
              juce::NormalisableRange{-12.f, 12.f, 0.1f, 0.5f}
              , 0.f,
              juce::AudioParameterFloatAttributes{}.withLabel("Db"));

      return addParameterToProcessor(processor, std::move(parameter));
  }

  juce::AudioParameterBool &createBypassedParameter(juce::AudioProcessor &processor){
      constexpr auto versionHint = 1;
      auto parameter = std::make_unique<juce::AudioParameterBool>(
              juce::ParameterID{"bypass.control", versionHint}
              ,"Bypass",
              false
              );
      return addParameterToProcessor(processor, std::move(parameter));
  }
}

Parameters::Parameters(juce::AudioProcessor& processor)
// TODO: create parameters
// TODO: retrieve references to parameters
// TODO: add parameters to the processor
  : rate{createModulationRateParameter(processor)},
  gain{createGainParameter(processor)},
  bypassed{createBypassedParameter(processor)}

{
  juce::ignoreUnused(processor);
}
}  // namespace tremolo
