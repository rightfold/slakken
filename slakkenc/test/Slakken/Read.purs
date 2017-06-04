module Test.Slakken.Read
  ( spec
  ) where

import Control.Comonad.Cofree ((:<))
import Data.ByteString as ByteString
import Slakken.AST (Value(..))
import Slakken.Read (read)
import Stuff
import Test.Spec (Spec, describe, it)
import Test.Spec.Assertions (fail)

--------------------------------------------------------------------------------

spec :: ∀ r. Spec r Unit
spec = describe "Slakken.Read" do
  readSpec

readSpec :: ∀ r. Spec r Unit
readSpec = describe "read" do
  example "Empty" "" (Right [])
  example "Only white space" " \t\r\n" (Right [])
  example "Array" "[] [[]] [[] [[]]]" $ Right
    [ 0 :< array []
    , 3 :< array [4 :< array []]
    , 8 :< array [9 :< array [], 12 :< array [13 :< array []]]
    ]
  example "Atom" "a bc def" $ Right
    [ 0 :< atom "a"
    , 2 :< atom "bc"
    , 5 :< atom "def"
    ]
  example "Float" "2f 3.14f 42f" $ Right
    [ 0 :< float 2.0
    , 3 :< float 3.14
    , 9 :< float 42.0
    ]
  where
  example n i o = it n $ read i `shouldEqual'` o
  shouldEqual' a b = when (a /= b) $ fail "Values are not equal"
  array = Array
  atom = Atom \ ByteString.toUTF8
  float = Float
