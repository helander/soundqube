/*
 */

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    MIDI_CONTROL_CHANGE = 0xB0,
    MIDI_PROGRAM_CHANGE = 0xC0,
} MidiSpec;

typedef enum {
    SETBFREE_MIDI_RANDOM_DRAWBARS = 55,
} SetBFreeSettings;

typedef struct {
    LV2_Atom_Event event;
    uint8_t        msg[3];
} LV2_Atom_MIDI;

typedef enum {
    PORT_ATOM_IN = 0,
    PORT_ATOM_OUT,

    PORT_CONTROL_FIRST,
    PORT_CONTROL_SEND_CONFIGURATION = PORT_CONTROL_FIRST,

    PORT_CONTROL_PRESET,
    PORT_CONTROL_LOWER_MANUAL_PRESET,
    PORT_CONTROL_UPPER_MANUAL_PRESET,

    PORT_CONTROL_RANDOM_DRAWBARS,

    PORT_CONTROL_FIRST_SETTING,
    PORT_CONTROL_OVERDRIVE_CHARACTER = PORT_CONTROL_FIRST_SETTING,
    PORT_CONTROL_REVERB_MIX,
    PORT_CONTROL_VOLUME,
    PORT_CONTROL_PERCUSSION_ENABLE,
    PORT_CONTROL_PERCUSSION_VOLUME,
    PORT_CONTROL_PERCUSSION_DECAY,
    PORT_CONTROL_PERCUSSION_HARMONIC,
    PORT_CONTROL_VIBRATO_LOWER,
    PORT_CONTROL_VIBRATO_UPPER,
    PORT_CONTROL_VIBRATO_KNOK,
    PORT_CONTROL_ROTARY_SPEED_PRESET,

    PORT_CONTROL_DRAWBAR_UPPER,
    PORT_CONTROL_DRAWBAR_UPPER_16 = PORT_CONTROL_DRAWBAR_UPPER,
    PORT_CONTROL_DRAWBAR_UPPER_513,
    PORT_CONTROL_DRAWBAR_UPPER_8,
    PORT_CONTROL_DRAWBAR_UPPER_4,
    PORT_CONTROL_DRAWBAR_UPPER_223,
    PORT_CONTROL_DRAWBAR_UPPER_2,
    PORT_CONTROL_DRAWBAR_UPPER_135,
    PORT_CONTROL_DRAWBAR_UPPER_113,
    PORT_CONTROL_DRAWBAR_UPPER_1,

    PORT_CONTROL_DRAWBAR_LOWER,
    PORT_CONTROL_DRAWBAR_LOWER_16 = PORT_CONTROL_DRAWBAR_LOWER,
    PORT_CONTROL_DRAWBAR_LOWER_513,
    PORT_CONTROL_DRAWBAR_LOWER_8,
    PORT_CONTROL_DRAWBAR_LOWER_4,
    PORT_CONTROL_DRAWBAR_LOWER_223,
    PORT_CONTROL_DRAWBAR_LOWER_2,
    PORT_CONTROL_DRAWBAR_LOWER_135,
    PORT_CONTROL_DRAWBAR_LOWER_113,
    PORT_CONTROL_DRAWBAR_LOWER_1,

    PORT_CONTROL_DRAWBAR_PEDAL,
    PORT_CONTROL_DRAWBAR_PEDAL_16 = PORT_CONTROL_DRAWBAR_PEDAL,
    PORT_CONTROL_DRAWBAR_PEDAL_513,
    PORT_CONTROL_DRAWBAR_PEDAL_8,
    PORT_CONTROL_DRAWBAR_PEDAL_4,
    PORT_CONTROL_DRAWBAR_PEDAL_223,
    PORT_CONTROL_DRAWBAR_PEDAL_2,
    PORT_CONTROL_DRAWBAR_PEDAL_135,
    PORT_CONTROL_DRAWBAR_PEDAL_113,
    PORT_CONTROL_DRAWBAR_PEDAL_1,

    // Note: it have to be the last
    PORT_ENUM_SIZE
} PortEnum;

typedef void(*ConvertFuncPtr_t)(float* const control, uint8_t* const midi, bool to_midi);

typedef struct {
    uint8_t channel;
    uint8_t control;
    ConvertFuncPtr_t convert;
} SetBFreeMidiConfig;

typedef struct {
    const float* port;
    float last_value;
    SetBFreeMidiConfig midi_config;
} Parameter;

typedef struct {
    // URIDs
    LV2_URID urid_midiEvent;

    // state
    // Note: it also alloc slots for MIDI in/out but we dont care
    Parameter parameters[PORT_ENUM_SIZE];

    // atom ports
    const LV2_Atom_Sequence* port_events_in;
    LV2_Atom_Sequence* port_events_out;

    // memory for MIDI message sending
    uint8_t msg_buffer[4 * PORT_ENUM_SIZE];
} Data;


void convert_8to0(float* const control, uint8_t* const midi, bool to_midi) {
    if (to_midi) {
        float c = *control;
        *midi = 127 - (int) ((c * 127) / 8);
    }
}

void convert_linear(float* const control, uint8_t* const midi, bool to_midi) {
    if (to_midi) {
        float c = *control;
        *midi = (int) (c * 127);
    }
}

void convert_0to1(float* const control, uint8_t* const midi, bool to_midi) {
    if (to_midi) {
        float c = *control;
        *midi = (c < 0.5)?0:127;
    }
}

void convert_0to2(float* const control, uint8_t* const midi, bool to_midi) {
    if (to_midi) {
        float c = *control;
        *midi = (int) ((c * 127) / 2);
    }
}

void convert_0to5(float* const control, uint8_t* const midi, bool to_midi) {
    if (to_midi) {
        float c = *control;
        *midi = (int) ((c * 127) / 5);
    }
}


static LV2_Handle instantiate(const LV2_Descriptor*     descriptor,
                              double                    rate,
                              const char*               path,
                              const LV2_Feature* const* features)
{
    Data* self = (Data*)calloc(1, sizeof(Data));

    for (int port = PORT_CONTROL_FIRST; port < PORT_ENUM_SIZE; port++) {
        Parameter *parameter = self->parameters + port;
        parameter->last_value = -1;
    }
    self->parameters[PORT_CONTROL_DRAWBAR_UPPER+0].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control = 70, .convert=convert_8to0};
    self->parameters[PORT_CONTROL_DRAWBAR_UPPER+1].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control = 71, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_UPPER+2].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control = 72, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_UPPER+3].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control = 73, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_UPPER+4].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control = 74, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_UPPER+5].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control = 75, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_UPPER+6].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control = 76, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_UPPER+7].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control = 77, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_UPPER+8].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control = 78, .convert=convert_8to0 };

    self->parameters[PORT_CONTROL_DRAWBAR_LOWER+0].midi_config = (SetBFreeMidiConfig) { .channel = 1, .control = 70, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_LOWER+1].midi_config = (SetBFreeMidiConfig) { .channel = 1, .control = 71, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_LOWER+2].midi_config = (SetBFreeMidiConfig) { .channel = 1, .control = 72, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_LOWER+3].midi_config = (SetBFreeMidiConfig) { .channel = 1, .control = 73, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_LOWER+4].midi_config = (SetBFreeMidiConfig) { .channel = 1, .control = 74, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_LOWER+5].midi_config = (SetBFreeMidiConfig) { .channel = 1, .control = 75, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_LOWER+6].midi_config = (SetBFreeMidiConfig) { .channel = 1, .control = 76, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_LOWER+7].midi_config = (SetBFreeMidiConfig) { .channel = 1, .control = 77, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_LOWER+8].midi_config = (SetBFreeMidiConfig) { .channel = 1, .control = 78, .convert=convert_8to0 };

    self->parameters[PORT_CONTROL_DRAWBAR_PEDAL+0].midi_config = (SetBFreeMidiConfig) { .channel = 2, .control = 70, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_PEDAL+1].midi_config = (SetBFreeMidiConfig) { .channel = 2, .control = 71, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_PEDAL+2].midi_config = (SetBFreeMidiConfig) { .channel = 2, .control = 72, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_PEDAL+3].midi_config = (SetBFreeMidiConfig) { .channel = 2, .control = 73, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_PEDAL+4].midi_config = (SetBFreeMidiConfig) { .channel = 2, .control = 74, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_PEDAL+5].midi_config = (SetBFreeMidiConfig) { .channel = 2, .control = 75, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_PEDAL+6].midi_config = (SetBFreeMidiConfig) { .channel = 2, .control = 76, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_PEDAL+7].midi_config = (SetBFreeMidiConfig) { .channel = 2, .control = 77, .convert=convert_8to0 };
    self->parameters[PORT_CONTROL_DRAWBAR_PEDAL+8].midi_config = (SetBFreeMidiConfig) { .channel = 2, .control = 78, .convert=convert_8to0 };

    self->parameters[PORT_CONTROL_OVERDRIVE_CHARACTER].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control = 93, .convert=convert_linear };
    self->parameters[PORT_CONTROL_REVERB_MIX].midi_config =          (SetBFreeMidiConfig) { .channel = 0, .control = 91, .convert=convert_linear };
    self->parameters[PORT_CONTROL_VOLUME].midi_config =              (SetBFreeMidiConfig) { .channel = 0, .control =  7, .convert=convert_linear };
    self->parameters[PORT_CONTROL_PERCUSSION_ENABLE].midi_config =   (SetBFreeMidiConfig) { .channel = 0, .control = 80, .convert=convert_0to1 };
    self->parameters[PORT_CONTROL_PERCUSSION_VOLUME].midi_config =   (SetBFreeMidiConfig) { .channel = 0, .control = 81, .convert=convert_0to1 };
    self->parameters[PORT_CONTROL_PERCUSSION_DECAY].midi_config =    (SetBFreeMidiConfig) { .channel = 0, .control = 82, .convert=convert_0to1 };
    self->parameters[PORT_CONTROL_PERCUSSION_HARMONIC].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control = 83, .convert=convert_0to1 };
    self->parameters[PORT_CONTROL_VIBRATO_LOWER].midi_config =       (SetBFreeMidiConfig) { .channel = 0, .control = 30, .convert=convert_0to1 };
    self->parameters[PORT_CONTROL_VIBRATO_UPPER].midi_config =       (SetBFreeMidiConfig) { .channel = 0, .control = 31, .convert=convert_0to1 };
    self->parameters[PORT_CONTROL_VIBRATO_KNOK].midi_config =        (SetBFreeMidiConfig) { .channel = 0, .control = 92, .convert=convert_0to5 };
    self->parameters[PORT_CONTROL_ROTARY_SPEED_PRESET].midi_config = (SetBFreeMidiConfig) { .channel = 0, .control =  1, .convert=convert_0to2 };

    self->parameters[PORT_CONTROL_SEND_CONFIGURATION].last_value = 0;
    self->parameters[PORT_CONTROL_PRESET].last_value = 0;
    self->parameters[PORT_CONTROL_LOWER_MANUAL_PRESET].last_value = 0;
    self->parameters[PORT_CONTROL_UPPER_MANUAL_PRESET].last_value = 0;
    self->parameters[PORT_CONTROL_RANDOM_DRAWBARS].last_value = 0;

    // Get host features
    LV2_URID_Map* urid_map = NULL;

    for (int i = 0; features[i]; ++i) {
        if (!strcmp(features[i]->URI, LV2_URID__map)) {
            urid_map = (LV2_URID_Map*)features[i]->data;
            break;
        }
    }

    if (!urid_map) {
        free(self);
        return NULL;
    }

    // Map URIs
    self->urid_midiEvent = urid_map->map(urid_map->handle, LV2_MIDI__MidiEvent);

    return self;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
    Data* self = (Data*)instance;

    switch (port)
    {
    case PORT_ATOM_IN:
            self->port_events_in = (const LV2_Atom_Sequence*)data;
            break;
    case PORT_ATOM_OUT:
            self->port_events_out = (LV2_Atom_Sequence*)data;
            break;
    default:
        self->parameters[port].port = (const float*)data;
    }
}

static void activate(LV2_Handle instance)
{
    // Data* self = (Data*)instance;
}

static uint8_t * write_midi_signal(Data* self, uint8_t *msg, int port)
{
    Parameter *parameter = self->parameters + port;

    switch (port) {
    case PORT_CONTROL_PRESET:
    case PORT_CONTROL_LOWER_MANUAL_PRESET:
    case PORT_CONTROL_UPPER_MANUAL_PRESET: {
        uint8_t value = (uint8_t) parameter->last_value;
        if (value != 0) {
            msg[0] = 2;
            msg[1] = MIDI_PROGRAM_CHANGE + 0;
            msg[2] = value - 1;
            msg += 4;
        }
        break;
    }
    case PORT_CONTROL_RANDOM_DRAWBARS: {
        uint8_t value = (uint8_t) parameter->last_value;
        if (value != 0) {
            msg[0] = 2;
            msg[1] = MIDI_PROGRAM_CHANGE + 0;
            msg[2] = SETBFREE_MIDI_RANDOM_DRAWBARS - 1;
            msg += 4;
        }
        break;
    }
    default: {
        SetBFreeMidiConfig *config = &parameter->midi_config;
        msg[0] = 3;
        msg[1] = MIDI_CONTROL_CHANGE + config->channel;
        msg[2] = config->control;
        config->convert(&parameter->last_value, &msg[3], true);
        msg += 4;
    }
    }
    return msg;
}


static void run(LV2_Handle instance, uint32_t sample_count)
{
    Data* self = (Data*)instance;
    uint8_t *msg = self->msg_buffer;
    uint8_t const *msg_start = self->msg_buffer;

    for (int port = PORT_CONTROL_FIRST; port < PORT_ENUM_SIZE; port++) {
        Parameter *parameter = self->parameters + port;

        // sync cache and port
        if (parameter->last_value == *parameter->port) {
            continue;
        }
        parameter->last_value = *parameter->port;

        if (port == PORT_CONTROL_SEND_CONFIGURATION) {
            // reset the current MIDI messages
            // and do not do anything else
            if (parameter->last_value == 0) {
                continue;
            }
            msg = self->msg_buffer;
            msg = write_midi_signal(self, msg, PORT_CONTROL_PRESET);
            msg = write_midi_signal(self, msg, PORT_CONTROL_LOWER_MANUAL_PRESET);
            msg = write_midi_signal(self, msg, PORT_CONTROL_UPPER_MANUAL_PRESET);
            if (msg != msg_start) {
                // break if there is any presets
                break;
            }

            // feed with all the settings
            for (int port = PORT_CONTROL_FIRST_SETTING; port < PORT_ENUM_SIZE; port++) {
                msg = write_midi_signal(self, msg, port);
            }
            break;
        }

        // make the event
        msg = write_midi_signal(self, msg, port);
    }

    if (msg != msg_start) {
        // get MIDI port ready
        const uint32_t capacity = self->port_events_out->atom.size;
        lv2_atom_sequence_clear(self->port_events_out);
        self->port_events_out->atom.type = self->port_events_in->atom.type;

        LV2_Atom_MIDI midimsg;
        memset(&midimsg, 0, sizeof(LV2_Atom_MIDI));

        for (uint8_t const *m = msg_start; m < msg; m += 4) {
            midimsg.event.body.size = m[0];
            midimsg.event.body.type = self->urid_midiEvent;
            midimsg.msg[0] = m[1];
            midimsg.msg[1] = m[2];
            midimsg.msg[2] = m[3];
            lv2_atom_sequence_append_event(self->port_events_out, capacity, &midimsg.event);
        }
    }
}

static void cleanup(LV2_Handle instance)
{
    free(instance);
}

static const LV2_Descriptor descriptor = {
    .URI = "http://helander.network/plugins/lv2/setbfreecontrol",
    .instantiate = instantiate,
    .connect_port = connect_port,
    .activate = activate,
    .run = run,
    .deactivate = NULL,
    .cleanup = cleanup,
    .extension_data = NULL
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    return (index == 0) ? &descriptor : NULL;
}
