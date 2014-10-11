module Main

import IR
import IR.Event
import IR.Layout
import IR.Lens
import IR.StackSet

%include C "cbits/wayland.h"
%link C "src/wayland.o"
%include C "cbits/ir.h"
%link C "src/ir.o"

%default total

partial
putErrLn : String -> IO ()
putErrLn s = fwrite stderr (s ++ "\n")

waylandInit : IO Bool
waylandInit = map (/= 0) (mkForeign (FFun "waylandInit" [] FInt))


WaylandWindow : Type
WaylandWindow = Int

waylandTileWindow : WaylandWindow -> Rectangle -> IO ()
waylandTileWindow wid r =
  mkForeign (FFun "waylandWindowSetRect" [FInt, FFloat, FFloat, FFloat, FFloat] FUnit) wid (rectX r) (rectY r) (rectW r) (rectH r)

waylandFocusWindow : WaylandWindow -> IO ()
waylandFocusWindow wid =
  mkForeign (FFun "waylandWindowSetFocus" [FInt] FUnit) wid


partial
main : IO ()
main = do
  putStrLn "iridium started"
  a <- waylandInit
  if not a
  then do
    putErrLn "iridium C component borked as expected <g>."
--  else runInit [(), !initialQuartzState, quartzConf] runIR
