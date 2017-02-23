import Data.List
import Debug.Trace

-- Problem 1
-- Return pair containing roots of quadratic equation a*x**2 + b*x + c.
-- The first element in the returned pair should use the positive 
-- square-root of the discriminant, the second element should use the 
-- negative square-root of the discriminant.  Need not handle complex
-- roots.
quadraticRoots :: Floating t => t -> t -> t -> (t, t)
quadraticRoots a b c =
  let firPart = (- b)
      secPart = sqrt ((b ^ 2) - (4 * a * c))
      
  in ((firPart + secPart) / (2 * a),(firPart - secPart) / (2 * a))

-- Problem 2
-- Return infinite list containing [z, f(z), f(f(z)), f(f(f(z))), ...]
-- May use recursion.
iterateFunction :: (a -> a) -> a -> [a]
iterateFunction f z = z : iterateFunction f (f z)
  

-- Problem 3
-- Using iterateFunction return infinite list containing 
-- multiples of n by all the non-negative integers.
-- May NOT use recursion.
multiples n = [ y * n| y <- iterateFunction (\x->x+1) 0]
  

-- Problem 4
-- Use iterateFunction to return an infinite list containing list 
-- of hailstone numbers starting with n.
-- Specifically, if i is a hailstone number, and i is even, then
-- the next hailstone number is i divided by 2; if i is a hailstone
-- number and i is odd, then the next hailstone number is 3*i + 1.
-- May NOT use recursion.
hailstones :: Integral a => a -> [a]
hailstones n = iterateFunction (\x -> if odd x then (3 * x + 1) else (x `div` 2)) n

-- Problem 5
-- Return length of hailstone sequence starting with n terminating
-- at the first 1.
-- May NOT use recursion.  Can use elemIndex from Data.List
hailstonesLen :: Integral a => a -> Int
hailstonesLen n = 
  if n > 0 
    then
      case elemIndex 1 (hailstones n) of
        Just index -> index + 1
        Nothing -> 0
    else 
      0

-- Problem 6
-- Given a list of numbers, return sum of the absolute difference
-- between consecutive elements of the list.
-- May NOT use recursion.
sumAbsDiffs :: Num a => [a] -> a
sumAbsDiffs numberList =
  if listLength > 1
    then
      let first = take (listLength - 1) numberList
          second = drop 1 numberList
      in sum (map (abs) (zipWith (-) first  second))
    else
      0
  where listLength = length numberList

-- Problem 7
-- The x-y coordinate of a point is represented using the pair (x, y).
-- Return the list containing the distance of each point in list
-- points from point pt.
-- May NOT use recursion.
distances :: Floating b => (b, b) -> [(b, b)] -> [b]
distances pt points =
  map (
    \(x1, y1) ->
      let firstPart = (x0 - x1) ^ 2
          secondPart= (y0 - y1) ^ 2
      in sqrt (firstPart + secondPart)) points 
  where (x0, y0) = pt

-- Problem 8
-- Given a list of coordinate pairs representing points, return the
-- sum of the lengths of all line segments between successive 
-- adjacent points.
-- May NOT use recursion.
sumLengths :: Floating a => [(a, a)] -> a
sumLengths pointsList =
  if listLength > 1
    then
      let first = take (listLength - 1) pointsList
          second = drop 1 pointsList
      in sum( zipWith (
        \(x1,y1) (x2,y2) -> 
          let [distance] = distances (x1,y1) [(x2,y2)] 
          in distance) first second)
    else
      0
  where listLength = length pointsList

-- Problem 9
-- Given a string s and char c, return list of indexes in s where c
-- occurs
-- occurrences :: [a] -> a -> [Int]
occurrences s c =  splitFunction s []
  where 
    splitFunction = (
      \splitString initialList ->  
          case elemIndex c splitString of
            Nothing -> initialList
            Just index -> 
              let toBeAdded = if null initialList then [index] else [(index + (last initialList)+ 1)]
              in splitFunction (drop (index + 1) splitString) (initialList ++ toBeAdded)
      )

-- A tree of some type t is either a Leaf containing a value of type t,
-- or it is an internal node (with constructor Tree) with some left
-- sub-tree, a value of type t and a right sub-tree.
data Tree t = Leaf t
            | Tree (Tree t) t (Tree t)

-- Problem 10
-- Fold tree to a single value.  If tree is a Tree node, then it's
-- folded value is the result of applying ternary treeFn to the result
-- of folding the left sub-tree, the value stored in the Tree node and
-- the result of folding the right sub-tree; if it is a Leaf node,
-- then the result of the fold is the result of applying the unary
-- leafFn to the value stored within the Leaf node.
-- May use recursion.
foldTree :: (t1 -> t -> t1 -> t1) -> (t -> t1) -> Tree t -> t1
foldTree treeFn leafFn tree =
  case tree of
    Leaf x -> leafFn x
    Tree tree1 tree2 tree3 -> treeFn (foldTree treeFn leafFn tree1) tree2 (foldTree treeFn leafFn tree3)

-- Problem 11
-- Return list containing flattening of tree.  The elements of the
-- list correspond to the elements stored in the tree ordered as per 
-- an in-order traversal of the tree. Must be implemented using foldTree.
-- May NOT use recursion.
flattenTree :: Tree a -> [a]
flattenTree tree = foldTree (\t1 t t2 -> t1 ++ [t] ++ t2) (\t1 -> [t1]) tree


-- Problem 12
-- Given tree of type (Tree [t]) return list which is concatenation
-- of all lists in tree.
-- Must be implemented using flattenTree.
-- May NOT use recursion.
catenateTreeLists :: Tree [a] -> [a]
catenateTreeLists tree = concat (flattenTree tree)
  
