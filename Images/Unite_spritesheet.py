import os
from PIL import Image
from PIL.ImageOps import mirror
from math import ceil

IMAGES_IN_ROW = 10
animation_directory = input("Animation directory: ")
#animation_directory = "Flametail/movement_0"
extension = '.png'
REVERSE = False

animation_fnames = sorted(filter(lambda f: f.endswith(extension), os.listdir(animation_directory)))
N = len(animation_fnames)
animation_images = [Image.open(os.path.join(animation_directory, f)) for f in animation_fnames]
im_size = [0, 0]
for im in animation_images:
    im_size[0] = max(im_size[0], im.size[0])
    im_size[1] = max(im_size[1], im.size[1])

current_corner = [0, 0]
animation_frames = [[0, 0, 0, 0] for f in animation_fnames]
column_j = 0
for i, im in enumerate(animation_images):
    animation_frames[i][2:4] = im_size
    animation_frames[i][:2] = current_corner
    if column_j < IMAGES_IN_ROW - 1:
        current_corner[0] += im_size[0]
        column_j += 1
    else:
        column_j = 0
        current_corner[0] = 0
        current_corner[1] += im_size[1]

total_im_size = (IMAGES_IN_ROW * im_size[0], ceil(N / IMAGES_IN_ROW) * im_size[1])
merged_image = Image.new('RGBA', total_im_size)
for fr, im in zip(animation_frames, animation_images):
    if REVERSE:
        merged_image.paste(mirror(im), fr[:2])
    else:
        merged_image.paste(im, fr[:2])

merged_image.save(animation_directory + '.png' if not REVERSE else animation_directory + 'reversed.png')