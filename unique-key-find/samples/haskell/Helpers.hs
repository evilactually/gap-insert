module Helpers (
  trKs,
  trDs,
  trVar,
  trVf,
  ksTr,
  ksMin,
  ksMax,
  useN,
  useN',
  getN,
  unuseNRandom
)
where

import Enum.Ranges
import RBST
import KeySpace

import System.Random

-- | Append value to list if it's not Nothing
(++?) :: Maybe a -> [a] -> [a]
(Just x) ++? xs = x:xs
(Nothing) ++? xs = xs

-- | Return list of all keys in a tree
trKs :: Tree a -> [a]
trKs E = []
trKs (T l k r _) = (trKs l) ++ [k] ++ (trKs r)

-- | Measure depth of each path
trDs :: Tree a -> Int -> [Int]
trDs E d = [d]
trDs (T l k r _) d = (trDs l (d + 1)) ++ (trDs r (d + 1))

-- | Maximum variance between dephs of shortest and longests paths
trVar tr = 
  let
    ds = trDs tr 0 
    min = minimum ds 
    max = maximum ds
    in (max - min)

-- | Verify order of nodes in a tree
trVf E = True
trVf (T E _ E _) = True
trVf (T _ k (T _ rk _ _) _) | not (k < rk) = error $ (show k) ++ " !< " ++ (show rk)
trVf (T (T _ lk _ _) k _ _) | not (k > lk) = error $ (show k) ++ " !> " ++ (show lk)
trVf (T _ _ _ _) = True

ksTr (KeySpace _ tr _ _) = tr

ksMin (KeySpace _ _ min _) = min

ksMax (KeySpace _ _ _ max) = max

useN 0 ks = ks
useN n ks = 
  useN (n-1) $ (fst.useUniqueKey) ks

-- | Use N unique keys
useN' 0 ks = ks
useN' n ks = 
  useN' (n-1) $ (fst.useUniqueKey') ks

-- | Get N unique keys
getN n ks = it n ks []
  where
    it 0 ks' iks = (ks', iks)
    it n ks' iks =
      let
        (ks'', ik) = useUniqueKey ks'
        in it (n-1) ks'' (ik++?iks) 

unuseNRandom g 0 ks = ks
unuseNRandom g n ks = 
  unuseNRandom ng (n-1) (unuseKey ks k)
  where
    (k, ng) = randomR (ksMin ks, ksMax ks) g