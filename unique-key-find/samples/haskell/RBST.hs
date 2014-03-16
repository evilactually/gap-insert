module RBST (
  Tree(T,E), 
  gapInsert,
  gapInsert',
  size, 
  remove, 
  contains
) 
where

import Enum.Ranges

import Data.Maybe
import System.Random

--               Abbreviations

-- tr - tree                   ng - new generator
-- ns - new size               li - left insertion
-- eg - either generator       ri - righ insertion
-- lg - left generator         ik inserted key
-- rg - right generator        k - key
-- lik - left inserted key     l - left node
-- rik - right inserted key    r - right node
-- tk - target key             g - generator            

data Tree a = E | T (Tree a) a (Tree a) Int deriving (Show)

-- | Fills any gap available in a tree and returns it's key in O(log N) time
--   Returns 0 if no gaps are available in constant time
--   Uses random generator to break ties when gaps are available on both sides 
gapInsert :: (Enum a, Eq a, RandomGen g) => g -> Tree a -> a -> a -> (g, Tree a, Maybe a)
gapInsert g tr min max
  | gaps > 0 = 
      case tr of
        E ->
          let 
            k = middleEnum min max
            in (g, (T E k E 1), Just k)
        T l k r _ -> 
          let
            ns = size(nl) + 1 + size(nr)
            (eg, nl, nr, ik) = 
              let
                (bias, ng) = random g
                (lg, li, lik) = (gapInsert ng l (min) (k))
                (rg, ri, rik) = (gapInsert ng r (k) (max))
                left = (bias && isJust lik) || (not bias && isNothing rik)
              in
                if(left)
                  then (lg, li, r, lik) 
                  else (rg, l, ri, rik)
            in (eg, (T nl k nr ns), ik)
  | otherwise = (g, tr, Nothing)
  where
    gaps = (enumsBetween min max) - size(tr)

--   (non-randomized version)
-- | Fills any gap available in a tree and returns it's key in O(log N) time
--   Returns 0 if no gaps are available in constant time
gapInsert' :: (Enum a, Eq a) => Tree a -> a -> a -> (Tree a, Maybe a)
gapInsert' tr min max
  | gaps > 0 = 
      case tr of
        E ->
          let 
            k = middleEnum min max
            in ((T E k E 1), Just k)
        T l k r _ -> 
          let
            ns = size(nl) + 1 + size(nr)
            (nl, nr, ik) = 
              let
                (li, lik) = (gapInsert' l (min) (k))
                (ri, rik) = (gapInsert' r (k) (max))
              in
                if(isJust lik)
                  then (li, r, lik)
                  else (l, ri, rik)
            in ((T nl k nr ns), ik)
  | otherwise = (tr, Nothing)
  where
    gaps = (enumsBetween min max) - size(tr)

size (T _ _ _ s) = s
size E = 0

-- | Removes nodes with a given key if it's found in a tree. 
--   Uses random generator to decide between deleting predecessor or successor
remove g E tk = (g, E)
remove g (T E k r _) tk | tk == k = (g, r)
remove g (T l k E _) tk | tk == k = (g, l)
remove g (T l k r _) tk | tk == k =
  let
    (left, ng) = random g
    (rl, lrk) = rm_pred l
    (rr, rrk) = rm_succ r
    (nl, nk, nr) =
      if left
        then (rl, lrk, r)
        else (l, rrk, rr)
    ns = (size nl) + 1 + (size nr)
  in (ng, (T nl nk nr ns))
  where
    rm_pred (T E k E _) = (E, k)
    rm_pred (T l k E _) = (l, k)
    rm_pred (T l k r _) = ((T nl k r ns), rk)
      where
        ns = (size nl) + 1 + (size r)
        (nl, rk) = rm_pred r
    rm_succ (T E k E _) = (E, k)
    rm_succ (T E k r _) = (r, k)
    rm_succ (T l k r _) = ((T nl k r ns), rk)
      where
        ns = (size nl) + 1 + (size r)
        (nl, rk) = rm_succ l
remove g (T l k r _) tk | tk > k = (ng, (T l k nr ns)) 
  where 
    ns = (size l) + 1 + (size nr) 
    (ng, nr) = remove g r tk
remove g (T l k r _) tk | tk < k = (ng, (T nl k r ns)) 
  where 
    ns = (size nl) + 1 + (size r) 
    (ng, nl) = remove g l tk

-- | Returns True if a tree contains a given key
--   Uses random generator when it doesn't know xD
contains :: (Ord a) => Tree a -> a -> Bool
contains E tk = False
contains (T _ k _ _) tk | tk == k = True
contains (T _ k r _) tk | tk > k = contains r tk
contains (T l k _ _) tk | tk < k = contains l tk
