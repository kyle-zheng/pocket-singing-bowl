English | [简体中文](docs/i18n/zh-CN/README.md)

<h1 align="center">Pocket Singing Bowl</h1>

<p align="center">A little moon. A golden bowl. A quiet moment in your pocket.</p>

<p align="center">
  <a href="https://github.com/kyle-zheng/pocket-singing-bowl/releases/tag/v0.1.8"><img src="assets/badges/version.svg" alt="Version: v0.1.8"></a>
  <a href="https://github.com/kyle-zheng/pocket-singing-bowl/blob/main/docs/USER_GUIDE.md"><img src="assets/badges/platform.svg" alt="Platform: FoloToy AI Passport"></a>
  <a href="https://github.com/kyle-zheng/pocket-singing-bowl/blob/main/docs/USER_GUIDE.md"><img src="assets/badges/sound.svg" alt="Sound: 7 tones"></a>
  <a href="https://github.com/kyle-zheng/pocket-singing-bowl/blob/main/docs/USER_GUIDE.md"><img src="assets/badges/mode.svg" alt="Mode: Offline"></a>
  <a href="https://github.com/kyle-zheng/pocket-singing-bowl/blob/main/LICENSE"><img src="assets/badges/license.svg" alt="License: MIT"></a>
</p>

An offline singing-bowl experience for [FoloToy AI Passport](https://github.com/FoloToy/ai-passport), inspired by Himalayan singing bowls from Nepal. I paired seven original synthesized tones with the seven chakras, creating selectable listening themes for everyday relaxation, focus, and meditation. Simple button controls make it easy to start listening.

<p align="center"><img src="assets/images/pocket-singing-bowl-device-preview.png" alt="Pocket Singing Bowl shown on FoloToy AI Passport, with a closer view of its full-moon interface" width="900"></p>

*AI-generated device illustration showing the app on FoloToy AI Passport; not an on-device photograph.*

<p align="center"><img src="assets/images/moon-bowl-source.png" alt="AI-generated artwork of a golden singing bowl beneath a full moon" width="300"></p>

[Download v0.1.8](https://github.com/kyle-zheng/pocket-singing-bowl/releases/tag/v0.1.8) · [Try it in your browser](https://apsimulator.folotoy.cn/?play=1384) · [Community project](https://ai-passport.folotoy.cn/plays/1384/) · [User guide](docs/USER_GUIDE.md) · [Build from source](docs/BUILDING.md)

## What it does

- Seven selectable notes and themes, from C/root to B/crown.
- Play a tone with one press, or let automatic playback create a listening rhythm.
- Set the strike interval from 6 to 60 seconds and choose a timed or unlimited session.
- Adjust volume from 0% (mute) to 100%; hold a button to adjust it faster.
- Optional sleep after ten idle minutes, with a gentle screen fade; disabled by default.
- Works entirely offline and remembers your settings.

The current firmware interface is in Simplified Chinese. Project documentation defaults to English, with a Simplified Chinese translation available. The artwork shown above is AI-generated; the bowls retain the circular full-moon emblem used in my original design. The sounds are original synthesized timbres rather than recordings of physical bowls. Theme and chakra names describe the design; they do not claim medical effects.

## Seven notes, seven chakras

For Pocket Singing Bowl, I use the following note-to-chakra mapping, with a listening theme for each note:

| Note | Chakra | Listening theme |
| --- | --- | --- |
| C | Root | Grounding & Relaxation |
| D | Sacral | Feeling & Flow |
| E | Solar plexus | Confidence & Action |
| F | Heart | Acceptance & Care |
| G | Throat | Expression & Listening |
| A | Third eye | Focus & Awareness |
| B | Crown | Stillness & Meditation |

## Start listening

1. Install the firmware on FoloToy AI Passport. No network setup is needed. For a ready-made installation, open the community project linked above; see the [installation guide](docs/INSTALL.md) for manual flashing.
2. Press **OK** once to play. Press **UP** or **DOWN** to change the note and theme.
3. Hold **OK** to start automatic playback. Press **OK** to pause or resume; hold it again to return to manual mode.
4. Double-press **OK** to open settings. Use **UP/DOWN** to select or adjust, and press **OK** to edit or save. Hold **OK** to cancel an edit or return from the settings list. When editing interval or volume, hold **UP/DOWN** for faster changes.

Opening settings pauses automatic playback. Press OK on the home screen to resume it after leaving settings.

## Defaults in v0.1.8

| Setting | Default |
| --- | --- |
| Note | B · Crown |
| Theme | Stillness & Meditation |
| Volume | 50% |
| Strike interval | 6 seconds |
| Automatic session | 3 minutes |
| Automatic sleep | Off |
| Startup | Silent, manual standby |

Saved preferences take precedence on an ordinary restart. A full merged-image refresh replaces the existing application and restores defaults; follow the installation guide before flashing.

## Explore the project

| Resource | Contents |
| --- | --- |
| [User guide](docs/USER_GUIDE.md) | Controls, themes, settings, and sleep behavior |
| [Installation guide](docs/INSTALL.md) | Install a complete image and verify its checksum |
| [Build guide](docs/BUILDING.md) | ESP-IDF setup, firmware build, and host checks |
| [Architecture](docs/ARCHITECTURE.md) | Controller, audio mixing, and power behavior |
| [Asset credits](docs/ASSETS.md) | Artwork, synthesized audio, font, and upstream credits |
| [Release notes](docs/RELEASE_NOTES.md) | Current release and user-visible changes |
| [License](LICENSE) | MIT license; the bundled font retains its separate license |

## Development status

I have flashed v0.1.8 to FoloToy AI Passport and tried it myself. It starts with B/crown at 50% volume and keeps the earlier sound and mixing improvements. After the tuning, I no longer hear the crackling from my earlier listening sessions.

## Credits

Built on the open FoloToy AI Passport firmware and board-support code. The original MIT notice is preserved in [LICENSE](LICENSE). See [asset credits](docs/ASSETS.md) for the font license and generated artwork. This is an independent community application.
