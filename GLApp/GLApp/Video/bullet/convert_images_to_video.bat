@echo off
ffmpeg -framerate 30 -i Picture_%%d.png -c:v libx264 -pix_fmt yuv420p output_video.mp4
