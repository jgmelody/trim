namespace tremolo {
namespace {
    struct SerializableParameters {
        float rate;
        float gain;
        bool bypassed;
        juce::String waveform;

        static constexpr auto marshallingVersion = 1;

        template <typename Archive, typename T>
        static void serialise(Archive& archive, T& t){

          using namespace juce;

          if(archive.getVersion()!= 1){
            return;
          }

          std::string pluginName = TREMOLO_PLUGIN_NAME;

          archive(named("pluginName", pluginName));

          if (pluginName != TREMOLO_PLUGIN_NAME) {
              return;
}

          archive(named("modulationRateHz", t.rate), named("gain", t.gain), named("bypassed", t.bypassed)
                  , named("modulationWaveform", t.waveform));

        }
    };

    SerializableParameters from(const tremolo::Parameters& parameters) {
      return {
              .rate = parameters.rate.get(),
              .gain = parameters.gain.get(),
              .bypassed = parameters.bypassed.get(),
              .waveform = parameters.waveform.getCurrentChoiceName(),
      };
    }
}

void JsonSerializer::serialize(const Parameters& parameters,
                               juce::OutputStream& output) {
  const auto parametersToSerialize = from(parameters);

  const auto json = juce::ToVar::convert(parametersToSerialize);

  if(!json.has_value()) {
    return;
  }

  juce::JSON::writeToStream(output, *json
                            ,juce::JSON::FormatOptions{}
                            .withSpacing(juce::JSON::Spacing::multiLine)
                            .withMaxDecimalPlaces(2));
}

juce::Result JsonSerializer::deserialize(juce::InputStream& input,
                                         Parameters& parameters) {

    juce::var parsedResult;
    const auto result = juce::JSON::parse(input.readEntireStreamAsString(),
                                          parsedResult);

    if (result.failed()) {
        return result;
    }

    const auto parsedParameters = juce::FromVar::convert<SerializableParameters>
            (parsedResult);
    if (!parsedParameters.has_value()) {
        return juce::Result::fail("failed to parse parameters from JSON representation");
    }

    const auto modulationWaveformIndex = parameters.waveform.choices.indexOf(
            parsedParameters->waveform);
    parameters.waveform = modulationWaveformIndex;
    parameters.rate = parsedParameters->rate;
    parameters.gain = parsedParameters->gain;
    parameters.bypassed = parsedParameters-> bypassed;

  return juce::Result::ok();
}
}  // namespace tremolo
