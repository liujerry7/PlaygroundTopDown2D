#pragma once

// Engine build options for this game. Jumble reads this file when it's built as part of this solution.

// No sound yet, so skip FMOD and the fmod64.dll it needs at runtime. Delete this line to turn audio on.
#define ENGINE_DISABLE_AUDIO
