"""Validate embedded audio boundaries and generated font coverage without extra packages."""
from pathlib import Path
import re
import struct
import wave
ROOT = Path(__file__).resolve().parent.parent
bank = (ROOT / 'assets/music/moon-bowls.pcm').read_bytes()
assert len(bank) == 7 * 16000 * 8 * 2
for index, note in enumerate('CDEFGAB'):
    with wave.open(str(ROOT / f'assets/music/bowl-{note}.wav'), 'rb') as audio:
        assert (audio.getnchannels(), audio.getsampwidth(), audio.getframerate(), audio.getnframes()) == (1, 2, 16000, 128000)
        raw = audio.readframes(128000)
    assert raw == bank[index*256000:(index+1)*256000]
    samples = struct.unpack('<128000h', raw)
    assert samples[0] == samples[-1] == 0
    assert 14500 < max(abs(x) for x in samples) <= 18350
    # Smooth onset; normal high-frequency oscillation is not a click.
    assert max(abs(x) for x in samples[:16]) < 300
    assert max(abs(b-a) for a,b in zip(samples[:31],samples[1:32])) < 100
    assert max(abs(x) for x in samples[-160:]) < 20
    assert abs(sum(samples)/len(samples)) < 10
# The exact Unicode set in the source must exist in every generated size.
text = (ROOT / 'main/moon_ui.c').read_text()
required = set(''.join(re.findall(r'"([^"\n]*)"', text)))
required = {ord(c) for c in required if ord(c) >= 32}
for size in (12, 14, 18, 28):
    source = (ROOT / f'assets/fonts/moon_font_{size}.c').read_text()
    offsets = re.search(r'unicode_list\[\]=\{([^}]+)\}', source).group(1)
    present = {int(n)+32 for n in offsets.split(',')}
    assert required <= present, (size, required-present)
print('Moon assets: PASS (7 PCM files, tail boundaries, headroom, all UI glyphs)')
