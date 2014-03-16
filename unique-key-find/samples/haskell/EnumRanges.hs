module Enum.Ranges (
  middleEnum,
  enumDistance,
  enumsBetween
)
where

middleEnum :: (Enum a) => a -> a -> a
middleEnum a b =
  toEnum(ia + (ib - ia) `div` 2)
  where
    ia = fromEnum a
    ib = fromEnum b

enumDistance :: (Enum a) => a -> a -> Int
enumDistance a b = abs $ (fromEnum b) - (fromEnum a)

enumsBetween :: (Enum a, Eq a) => a -> a -> Int
enumsBetween a b
  | a == b = 0
  | otherwise = (enumDistance a b) - 1