#ifndef PLUGIN_URIS_H
#define PLUGIN_URIS_H

#include "lv2/atom/atom.h"
#include "lv2/midi/midi.h"
#include "lv2/patch/patch.h"
#include "lv2/urid/urid.h"

#define PLUGIN_URI "http://helander.network/plugins/lv2/unison"

typedef struct {
  LV2_URID atom_Path;
  LV2_URID atom_Resource;
  LV2_URID atom_Sequence;
  LV2_URID atom_URID;
  LV2_URID atom_eventTransfer;
  LV2_URID midi_Event;
  LV2_URID patch_Set;
  LV2_URID patch_property;
  LV2_URID patch_value;
} PluginURIs;

static inline void
map_plugin_uris(LV2_URID_Map* map, PluginURIs* uris)
{
  uris->atom_Path          = map->map(map->handle, LV2_ATOM__Path);
  uris->atom_Resource      = map->map(map->handle, LV2_ATOM__Resource);
  uris->atom_Sequence      = map->map(map->handle, LV2_ATOM__Sequence);
  uris->atom_URID          = map->map(map->handle, LV2_ATOM__URID);
  uris->atom_eventTransfer = map->map(map->handle, LV2_ATOM__eventTransfer);
  uris->midi_Event         = map->map(map->handle, LV2_MIDI__MidiEvent);
  uris->patch_Set          = map->map(map->handle, LV2_PATCH__Set);
  uris->patch_property     = map->map(map->handle, LV2_PATCH__property);
  uris->patch_value        = map->map(map->handle, LV2_PATCH__value);
}

#endif // PLUGIN_URIS_H
