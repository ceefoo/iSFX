import curses
import iSFX

system = iSFX.System()
sound = iSFX.Sound(system, "../../songs/private eyes.mp3")
sound.play()


stdscr = curses.initscr()
stdscr.timeout(10)
while 1:
  c = stdscr.getch()
  system.Update()
  if (c == -1):
    continue
  if (chr(c) == 'p'):
    sound.play()
  elif (chr(c) == 's'):
    sound.stop()
  elif (chr(c) == 'u'):
    sound.unpause()
  elif (chr(c) == 'q'):
    break
  elif (chr(c) == 'f'):
    sound.fade(-1.0, 1000)
  elif (chr(c) == 'g'):
    sound.fade(1.0, 1000)
  elif (chr(c) == 'r'):
    sound.fade(-1.0, 1000, then="pause")
  elif (chr(c) == 't'):
    sound.fade(-1.0, 1000, then="stop")
curses.endwin()