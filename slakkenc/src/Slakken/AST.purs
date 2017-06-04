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

derive instance fv :: Functor Value

instance ev :: Eq1 Value where
  eq1 (Array x) (Array y) = x == y
  eq1 (Atom x) (Atom y) = x == y
  eq1 (Float x) (Float y) = x == y
  eq1 _ _ = false

instance sv :: (Show a) => Show (Value a) where
  show (Array x) = "(Array " <> show x <> ")"
  show (Atom x) = "(Atom " <> show x <> ")"
  show (Float x) = "(Float " <> show x <> ")"

--------------------------------------------------------------------------------

data Expr f a
  = Var (Either Name String)
  | App Name (Array a)
  | Abs (Array String) a
  | Quote (f Value)

derive instance fe :: Functor (Expr f)

free :: ∀ f t. Recursive t (Expr f) => t -> Cofree (Expr f) (Set String)
free = cata $ flip (:<) <*> case _ of
  Var (Left _) -> Set.empty
  Var (Right n) -> Set.singleton n
  App _ as -> foldMap extract as
  Abs ns a -> foldr Set.delete (extract a) ns
  Quote _ -> Set.empty
