#!/usr/bin/env python3
"""Original speaker-oriented modal synthesis; no recording or treatment claim."""
from pathlib import Path
import json
import wave
import numpy as np
A = Path(__file__).resolve().parent.parent / 'assets'

def generate_audio():
    rate=16000; count=rate*8; t=np.arange(count)/rate
    freqs=[261.62556530,293.66476792,329.62755692,349.22823144,391.99543598,440,493.88330126]
    # Provisional levels for listening tests, not measured speaker calibration.
    peaks=[.46,.48,.50,.52,.54,.55,.56]
    fundamentals=[.62,.67,.72,.77,.82,.87,.92]
    bank=[]; report=[]
    for note,f,peak,fundamental in zip('CDEFGAB',freqs,peaks,fundamentals):
        signal=np.zeros(count)
        for ratio,amp,decay in [(1,fundamental,3.1),(2.71,.28,1.9),(5.13,.09,1.05),(8.42,.025,.62)]:
            signal+=amp*np.exp(-t/decay)*(np.sin(2*np.pi*f*ratio*t)+.42*np.sin(2*np.pi*(f*ratio+.65)*t))
        # Raised-cosine 24 ms mallet onset, eight-second resonant tail.
        signal*=.5-.5*np.cos(np.pi*np.minimum(t/.024,1))
        signal*=np.sin(np.minimum(1,(8-t)/.6)*np.pi/2)**2
        signal=signal/np.max(np.abs(signal))*peak
        pcm=np.round(signal*32767).astype('<i2'); pcm[0]=pcm[-1]=0
        with wave.open(str(A/f'music/bowl-{note}.wav'),'wb') as w:
            w.setparams((1,2,rate,0,'NONE','not compressed')); w.writeframes(pcm.tobytes())
        bank.append(pcm.tobytes())
        report.append({'note':note,'fundamental_hz':f,'peak':int(np.max(np.abs(pcm))), 'samples':count,'fundamental_weight':fundamental,'attack_ms':24})
    (A/'music/moon-bowls.pcm').write_bytes(b''.join(bank))
    return report

if __name__ == '__main__':
    p=A/'moon-assets-report.json'; report=json.loads(p.read_text())
    report['audio']=generate_audio(); p.write_text(json.dumps(report,indent=2))
    print('Audio assets generated: seven notes, 24 ms attack, individual headroom')
