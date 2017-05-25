module Slakken.AST
  ( Name(..)

  , Value(..)

  , Expr(..)
  , free
  ) where

import Data.ByteString (ByteString)
import Data.Set (Set)
import Data.Set as Set
import Control.Comonad.Cofree (Cofree, (:<))
import Matryoshka (class Recursive, cata)
import Stuff

--------------------------------------------------------------------------------

data Name =
  Name (Array String) String

--------------------------------------------------------------------------------

data Value a
  = Array (Array a)
  | Atom ByteString
  | Float Number

--------------------------------------------------------------------------------

data Expr a
  = Var (Either Name String)
  | App Name (Array a)
  | Abs (Array String) a
  | Quote (Mu Value)

derive instance fe :: Functor Expr

free :: ∀ t. Recursive t Expr => t -> Cofree Expr (Set String)
free = cata $ flip (:<) <*> case _ of
  Var (Left _) -> Set.empty
  Var (Right n) -> Set.singleton n
  App _ as -> foldMap extract as
  Abs ns a -> foldr Set.delete (extract a) ns
  Quote _ -> Set.empty
