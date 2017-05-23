module Slakken.AST
  ( Name(..)

  , Expr(..)
  , free
  ) where

import Data.Set (Set)
import Data.Set as Set
import Control.Comonad.Cofree (Cofree, (:<))
import Matryoshka (class Recursive, cata)
import Stuff

--------------------------------------------------------------------------------

data Name =
  Name (Array String) String

--------------------------------------------------------------------------------

data Expr a
  = Var (Either Name String)
  | App Name (Array a)
  | Abs (Array String) a

derive instance fe :: Functor Expr

free :: ∀ t. Recursive t Expr => t -> Cofree Expr (Set String)
free = cata $ flip (:<) <*> case _ of
  Var (Left _) -> Set.empty
  Var (Right n) -> Set.singleton n
  App _ as -> foldMap extract as
  Abs ns a -> foldr Set.delete (extract a) ns
