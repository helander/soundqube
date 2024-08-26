
#include "./uris.h"

#include "lv2/atom/atom.h"
#include "lv2/atom/util.h"
#include "lv2/core/lv2.h"
#include "lv2/core/lv2_util.h"
#include "lv2/log/log.h"
#include "lv2/log/logger.h"
#include "lv2/midi/midi.h"
#include "lv2/urid/urid.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum { PLUGIN_IN = 0,
       PLUGIN_OUT = 1,
       PLUGIN_CONTROL_C1 = 2,
       PLUGIN_CONTROL_C2 = 3,
       PLUGIN_CONTROL_C3 = 4,
       PLUGIN_CONTROL_C4 = 5,
       PLUGIN_CONTROL_C5 = 6,
       PLUGIN_CONTROL_C6 = 7,
       PLUGIN_CONTROL_C7 = 8,
       PLUGIN_CONTROL_C8 = 9
 };

typedef struct {
  // Features
  LV2_URID_Map*  map;
  LV2_Log_Logger logger;

  // Ports
  const LV2_Atom_Sequence* in_port;
  LV2_Atom_Sequence*       out_port;
  float* control_port_c1;
  float* control_port_c2;
  float* control_port_c3;
  float* control_port_c4;
  float* control_port_c5;
  float* control_port_c6;
  float* control_port_c7;
  float* control_port_c8;

  // URIs
  PluginURIs uris;
} Plugin;

static void
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
  Plugin* self = (Plugin*)instance;
  switch (port) {
  case PLUGIN_IN:
    self->in_port = (const LV2_Atom_Sequence*)data;
    break;
  case PLUGIN_OUT:
    self->out_port = (LV2_Atom_Sequence*)data;
    break;
  case PLUGIN_CONTROL_C1:
    self->control_port_c1 = (float *)data;
    break;
  case PLUGIN_CONTROL_C2:
    self->control_port_c2 = (float *)data;
    break;
  case PLUGIN_CONTROL_C3:
    self->control_port_c3 = (float *)data;
    break;
  case PLUGIN_CONTROL_C4:
    self->control_port_c4 = (float *)data;
    break;
  case PLUGIN_CONTROL_C5:
    self->control_port_c5 = (float *)data;
    break;
  case PLUGIN_CONTROL_C6:
    self->control_port_c6 = (float *)data;
    break;
  case PLUGIN_CONTROL_C7:
    self->control_port_c7 = (float *)data;
    break;
  case PLUGIN_CONTROL_C8:
    self->control_port_c8 = (float *)data;
    break;
  default:
    break;
  }
}

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               path,
            const LV2_Feature* const* features)
{
  // Allocate and initialise instance structure.
  Plugin* self = (Plugin*)calloc(1, sizeof(Plugin));
  if (!self) {
    return NULL;
  }

  // Scan host features for URID map
  // clang-format off
  const char*  missing = lv2_features_query(
    features,
    LV2_LOG__log,  &self->logger.log, false,
    LV2_URID__map, &self->map,        true,
    NULL);
  // clang-format on

  lv2_log_logger_set_map(&self->logger, self->map);
  if (missing) {
    lv2_log_error(&self->logger, "Missing feature <%s>\n", missing);
    free(self);
    return NULL;
  }

  map_plugin_uris(self->map, &self->uris);

  return (LV2_Handle)self;
}

static void
cleanup(LV2_Handle instance)
{
  free(instance);
}

static void
run(LV2_Handle instance, uint32_t sample_count)
{
  Plugin*     self = (Plugin*)instance;
  PluginURIs* uris = &self->uris;

  // Struct for a 3 byte MIDI event, used for writing notes
  typedef struct {
    LV2_Atom_Event event;
    uint8_t        msg[3];
  } MIDINoteEvent;

  // Initially self->out_port contains a Chunk with size set to capacity

  // Get the capacity
  const uint32_t out_capacity = self->out_port->atom.size;

  // Write an empty Sequence header to the output
  lv2_atom_sequence_clear(self->out_port);
  self->out_port->atom.type = self->in_port->atom.type;

  // Read incoming events
  LV2_ATOM_SEQUENCE_FOREACH (self->in_port, ev) {
    if (ev->body.type == uris->midi_Event) {
      const uint8_t* const msg = (const uint8_t*)(ev + 1);
      switch (lv2_midi_message_type(msg)) {
      case LV2_MIDI_MSG_NOTE_ON:
      case LV2_MIDI_MSG_NOTE_OFF:

        if (*(self->control_port_c1) > 0.5) {
          MIDINoteEvent note;

          note.event.time.frames = ev->time.frames; // Same time
          note.event.body.type   = ev->body.type;   // Same type
          note.event.body.size   = ev->body.size;   // Same size

          note.msg[0] = (msg[0] & 0xf0) | 0x00;     // Set channel
          note.msg[1] = msg[1];     // Same key
          note.msg[2] = msg[2];     // Same velocity

          lv2_atom_sequence_append_event(self->out_port, out_capacity, &note.event);
        }
        if (*(self->control_port_c2) > 0.5) {
          MIDINoteEvent note;

          note.event.time.frames = ev->time.frames; // Same time
          note.event.body.type   = ev->body.type;   // Same type
          note.event.body.size   = ev->body.size;   // Same size

          note.msg[0] = (msg[0] & 0xf0 )| 0x01;     // Set channel
          note.msg[1] = msg[1];     // Same key
          note.msg[2] = msg[2];     // Same velocity

          lv2_atom_sequence_append_event(self->out_port, out_capacity, &note.event);
        }
        if (*(self->control_port_c3) > 0.5) {
          MIDINoteEvent note;

          note.event.time.frames = ev->time.frames; // Same time
          note.event.body.type   = ev->body.type;   // Same type
          note.event.body.size   = ev->body.size;   // Same size

          note.msg[0] = (msg[0] & 0xf0 )| 0x02;     // Set channel
          note.msg[1] = msg[1];     // Same key
          note.msg[2] = msg[2];     // Same velocity

          lv2_atom_sequence_append_event(self->out_port, out_capacity, &note.event);
        }
        if (*(self->control_port_c4) > 0.5) {
          MIDINoteEvent note;

          note.event.time.frames = ev->time.frames; // Same time
          note.event.body.type   = ev->body.type;   // Same type
          note.event.body.size   = ev->body.size;   // Same size

          note.msg[0] = (msg[0] & 0xf0) | 0x03;     // Set channel
          note.msg[1] = msg[1];     // Same key
          note.msg[2] = msg[2];     // Same velocity

          lv2_atom_sequence_append_event(self->out_port, out_capacity, &note.event);
        }
        if (*(self->control_port_c5) > 0.5) {
          MIDINoteEvent note;

          note.event.time.frames = ev->time.frames; // Same time
          note.event.body.type   = ev->body.type;   // Same type
          note.event.body.size   = ev->body.size;   // Same size

          note.msg[0] = (msg[0] & 0xf0) | 0x04;     // Set channel
          note.msg[1] = msg[1];     // Same key
          note.msg[2] = msg[2];     // Same velocity

          lv2_atom_sequence_append_event(self->out_port, out_capacity, &note.event);
        }
        if (*(self->control_port_c6) > 0.5) {
          MIDINoteEvent note;

          note.event.time.frames = ev->time.frames; // Same time
          note.event.body.type   = ev->body.type;   // Same type
          note.event.body.size   = ev->body.size;   // Same size

          note.msg[0] = (msg[0] & 0xf0) | 0x05;     // Set channel
          note.msg[1] = msg[1];     // Same key
          note.msg[2] = msg[2];     // Same velocity

          lv2_atom_sequence_append_event(self->out_port, out_capacity, &note.event);
        }
        if (*(self->control_port_c7) > 0.5) {
          MIDINoteEvent note;

          note.event.time.frames = ev->time.frames; // Same time
          note.event.body.type   = ev->body.type;   // Same type
          note.event.body.size   = ev->body.size;   // Same size

          note.msg[0] = (msg[0] & 0xf0) | 0x06;     // Set channel
          note.msg[1] = msg[1];     // Same key
          note.msg[2] = msg[2];     // Same velocity

          lv2_atom_sequence_append_event(self->out_port, out_capacity, &note.event);
        }
        if (*(self->control_port_c8) > 0.5) {
          MIDINoteEvent note;

          note.event.time.frames = ev->time.frames; // Same time
          note.event.body.type   = ev->body.type;   // Same type
          note.event.body.size   = ev->body.size;   // Same size

          note.msg[0] = (msg[0] & 0xf0) | 0x07;     // Set channel
          note.msg[1] = msg[1];     // Same key
          note.msg[2] = msg[2];     // Same velocity

          lv2_atom_sequence_append_event(self->out_port, out_capacity, &note.event);
        }
        break;
      default:
        // Forward all other MIDI events directly
        lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
        break;
      }
    }
  }
}

static const void*
extension_data(const char* uri)
{
  return NULL;
}

static const LV2_Descriptor descriptor = {PLUGIN_URI,
                                          instantiate,
                                          connect_port,
                                          NULL, // activate,
                                          run,
                                          NULL, // deactivate,
                                          cleanup,
                                          extension_data};

LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
  return index == 0 ? &descriptor : NULL;
}

