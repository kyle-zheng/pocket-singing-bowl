#!/usr/bin/env python3
"""Reproducible offline assets. Requires Pillow, numpy and fonttools."""
from pathlib import Path
import json, re, wave
import numpy as np
from PIL import Image, ImageFont, ImageDraw
from fontTools.ttLib import TTFont
ROOT = Path(__file__).resolve().parent.parent
A = ROOT/'assets'
font_path = A/'fonts/NotoSansCJKsc-Regular.otf'
text = (ROOT/'main/moon_ui.c').read_text()
chars = sorted(set(''.join(re.findall(r'"([^"\n]*)"',text)) + ''.join(chr(i) for i in range(32,127))))
chars = [c for c in chars if ord(c)>=32]
cmap = TTFont(font_path).getBestCmap()
assert not [c for c in chars if ord(c) not in cmap], 'Missing glyphs'

def cbytes(data):
    return '\n'.join(','.join(str(v) for v in data[i:i+32])+',' for i in range(0,len(data),32))
for size in (12,14,18,28):
    font=ImageFont.truetype(str(font_path),size)
    bitmap=bytearray(); glyphs=['{0}']; unicode_list=[]
    for c in chars:
        x0,y0,x1,y1=font.getbbox(c,anchor='ls'); w=x1-x0; h=y1-y0
        im=Image.new('L',(max(1,w),max(1,h)))
        ImageDraw.Draw(im).text((-x0,-y0),c,font=font,fill=255,anchor='ls')
        pixels=(np.asarray(im).reshape(-1)//17).tolist() if w*h else []
        offset=len(bitmap)
        if len(pixels)%2: pixels.append(0)
        bitmap.extend((pixels[i]<<4)|pixels[i+1] for i in range(0,len(pixels),2))
        glyphs.append('{.bitmap_index=%d,.adv_w=%d,.box_w=%d,.box_h=%d,.ofs_x=%d,.ofs_y=%d}'%(offset,round(font.getlength(c)*16),w,h,x0,-y1))
        unicode_list.append(ord(c)-32)
    src='#include "lvgl.h"\nstatic const uint8_t bitmap[]={\n'+cbytes(bitmap)+'\n};\n'
    src+='static const lv_font_fmt_txt_glyph_dsc_t glyphs[]={'+',\n'.join(glyphs)+'};\n'
    src+='static const uint16_t unicode_list[]={'+','.join(map(str,unicode_list))+'};\n'
    src+='static const lv_font_fmt_txt_cmap_t cmaps[]={{.range_start=32,.range_length=%d,.glyph_id_start=1,.unicode_list=unicode_list,.list_length=%d,.type=LV_FONT_FMT_TXT_CMAP_SPARSE_TINY}};\n'%(ord(chars[-1])-31,len(chars))
    src+='static const lv_font_fmt_txt_dsc_t dsc={.glyph_bitmap=bitmap,.glyph_dsc=glyphs,.cmaps=cmaps,.cmap_num=1,.bpp=4,.bitmap_format=0};\n'
    src+='const lv_font_t moon_font_%d={.get_glyph_dsc=lv_font_get_glyph_dsc_fmt_txt,.get_glyph_bitmap=lv_font_get_bitmap_fmt_txt,.line_height=%d,.base_line=3,.dsc=&dsc};\n'%(size,size+4)
    (A/f'fonts/moon_font_{size}.c').write_text(src)
# Image remains flash-resident RGB565; no full framebuffer allocation.
im=Image.open(A/'images/moon-bowl-source.png').convert('RGB').resize((240,320),Image.Resampling.LANCZOS)
im.save(A/'images/moon-bowl-240.png')
p=np.asarray(im,dtype=np.uint16); rgb=((p[:,:,0]>>3)<<11)|((p[:,:,1]>>2)<<5)|(p[:,:,2]>>3)
raw=rgb.astype('<u2').tobytes()
(A/'images/moon_background.c').write_text('#include "lvgl.h"\nstatic const uint8_t pixels[] __attribute__((aligned(4)))={\n'+cbytes(raw)+'\n};\nconst lv_image_dsc_t moon_background={.header={.magic=LV_IMAGE_HEADER_MAGIC,.cf=LV_COLOR_FORMAT_RGB565,.w=240,.h=320,.stride=480},.data_size=sizeof(pixels),.data=pixels};\n')
from generate_moon_audio import generate_audio
report = generate_audio()
(A/'moon-assets-report.json').write_text(json.dumps({'glyph_count_per_size':len(chars),'image_bytes':len(raw),'audio':report},indent=2))
print('Assets PASS:',len(chars),'glyphs per font; image',len(raw),'bytes')
