{-# LANGUAGE GADTs #-}

module KeySpace (
  KeySpace(KeySpace), 
  emptyKeySpace, 
  useUniqueKey,
  useUniqueKey', 
  unuseKey, 
  unusedKeys, 
  usedKeys
) 
where

import Enum.Ranges
import RBST

import System.Random

data KeySpace a where
  KeySpace :: (RandomGen g, Enum a, Ord a, Eq a, Show a) => g -> Tree a -> a -> a -> KeySpace a

instance Show (KeySpace e) where
  show ks@(KeySpace _ _ min max) = "{ " ++ 
                                   "minimum: " ++ (show min) ++ ", " ++
                                   "maximum: " ++ (show max) ++ ", " ++
                                   "used keys: " ++ (show (usedKeys ks)) ++ ", " ++
                                   "unused keys: " ++ (show (unusedKeys ks)) ++ 
                                   " }"

emptyKeySpace min max = KeySpace (mkStdGen 42) E min max

useUniqueKey :: KeySpace a -> (KeySpace a, Maybe a)
useUniqueKey (KeySpace g tr min max) = 
  (KeySpace ng ntr min max, nk)
  where 
    (ng, ntr, nk) = gapInsert g tr min max

useUniqueKey' :: KeySpace a -> (KeySpace a, Maybe a)
useUniqueKey' (KeySpace g tr min max) = 
  (KeySpace g ntr min max, nk)
  where 
    (ntr, nk) = gapInsert' tr min max

unuseKey :: KeySpace a -> a -> KeySpace a
unuseKey (KeySpace g tr min max) k =
  KeySpace ng ntr min max
  where 
    (ng, ntr) = remove g tr k

isKeyUsed :: KeySpace a -> a -> Bool
isKeyUsed (KeySpace _ t _ _) k = contains t k 

unusedKeys :: KeySpace a -> Int
unusedKeys (KeySpace _ t min max) = (enumsBetween min max) - size(t)

usedKeys :: KeySpace a -> Int
usedKeys (KeySpace _ t _ _) = size(t)                                                                      