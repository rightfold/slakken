module Test.Main
  ( main
  ) where

import Control.Monad.Eff (Eff)
import Stuff
import Test.Slakken.AST as Slakken.AST
import Test.Spec.Reporter.Tap (tapReporter)
import Test.Spec.Runner (RunnerEffects, run)

main :: Eff (RunnerEffects ()) Unit
main = run [tapReporter] do
  Slakken.AST.spec
