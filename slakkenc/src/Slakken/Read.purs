module Slakken.Read
  ( read
  ) where

import Control.Comonad.Cofree (Cofree, (:<))
import Data.Array as Array
import Data.ByteString as ByteString
import Data.Number as Number
import Slakken.AST (Value(..))
import Stuff
import Text.Parsing.StringParser (ParseError, Parser(..), Pos, fail, runParser)
import Text.Parsing.StringParser.Combinators as PC
import Text.Parsing.StringParser.String as PS

--------------------------------------------------------------------------------

read :: String -> Either ParseError (Array (Cofree Value Pos))
read = runParser (whiteSpace *> many (force value) <* PS.eof)

value :: Lazy (Parser (Cofree Value Pos))
value = defer \_ -> force array <|> force atom <|> force float

array :: Lazy (Parser (Cofree Value Pos))
array = defer \_ -> do
  pos <- lexeme $ getPos <* PS.char '['
  elements <- many (force value)
  _ <- lexeme $ PS.char ']'
  pure $ pos :< Array elements

atom :: Lazy (Parser (Cofree Value Pos))
atom = defer \_ -> lexeme $ withPos $
  Atom \ ByteString.toUTF8 <$> PS.regex "[a-zA-Z-]+"

float :: Lazy (Parser (Cofree Value Pos))
float = defer \_ -> lexeme $ withPos $
  PS.regex "\\d+(\\.\\d+)?f"
  <#> Number.fromString
  >>= maybe (fail "Float is out of bounds") (pure \ Float)

--------------------------------------------------------------------------------

lexeme :: ∀ a. Parser a -> Parser a
lexeme = (_ <* whiteSpace)

whiteSpace :: Parser Unit
whiteSpace = void PS.whiteSpace

getPos :: Parser Pos
getPos = Parser \{str, pos} -> Right {result: pos, suffix: {str, pos}}

withPos :: Parser (Value (Cofree Value Pos)) -> Parser (Cofree Value Pos)
withPos = lift2 (:<) getPos

many :: ∀ a. Parser a -> Parser (Array a)
many = map Array.fromFoldable <<< PC.many
