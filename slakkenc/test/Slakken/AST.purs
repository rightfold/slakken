module Test.Slakken.AST
  ( spec
  ) where

import Data.Set as Set
import Slakken.AST (Expr(..), Name(..), free)
import Stuff
import Test.Spec (Spec, describe, it)
import Test.Spec.Assertions (shouldEqual)

--------------------------------------------------------------------------------

spec :: ∀ r. Spec r Unit
spec = describe "Slakken.AST" do
  freeSpec

freeSpec :: ∀ r. Spec r Unit
freeSpec = describe "free" do
  describe "Var" do
    example "Global" (In $ Var (Left (Name [] "x"))) []
    example "Local" (In $ Var (Right "x")) ["x"]
  example "App" (In $ App (Name [] "x") [In $ Var (Right "y")]) ["y"]
  describe "Abs" do
    example "Shadow" (In $ Abs ["x", "y"] (In $ Var (Right "x"))) []
    example "No Shadow" (In $ Abs ["x", "y"] (In $ Var (Right "z"))) ["z"]
  where
  example name node expect =
    it name $ extract (free node) `shouldEqual` Set.fromFoldable expect
