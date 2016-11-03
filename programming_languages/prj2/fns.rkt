;;-*- mode: scheme; -*-
;; :set filetype=scheme

;;Return a 2-element list containing the 2 roots of the quadratic
;;equation a*x^2 + b*x + c = 0 using the classical formula for the
;;roots of a quadratic equation.  The first element in the returned list
;;should use the positive square-root of the discriminant, the second
;;element should use the negative square-root of the discriminant.
(define (quadratic-roots a b c)
  (let ([rhs ( / (sqrt (- (sqr b) (* 4 (* a c)))) (* 2 a))] [lhs (/ (- b) (* 2 a))])
    (list (+ lhs rhs) (- lhs rhs))
  )
)

;;Return the list resulting by multiplying each element of `list` by `x`.
(define (mul-list list x)
  (if (null? list)
    '()
    (cons (* x (car list)) (mul-list (cdr list) x))
  )
)


;;Given a proper-list list of proper-lists, return the sum of the
;;lengths of all the contained lists.
(define (sum-lengths list)
  (if (null? list)
    0
    (+ (length(car list)) (sum-lengths (cdr list)))  
  )
)

;;Evaluate polynomial with list of coefficients coeffs (highest-order
;;first) at x.  The computation should reflect the traditional
;;representation of the polynomial.
(define (poly-eval coeffs x)
  (if (null? coeffs)
    0
    (let ([n (- (length coeffs) 1)] [c (car coeffs)])
      (+ (* c (expt x n)) (poly-eval (cdr coeffs) x))
    )  
  )
)

;;Evaluate polynomial with list of coefficients coeffs (highest-order
;;first) at x using Horner's method.
(define (poly-eval-horner coeffs x)
  (letrec ([eval (lambda (intialvalue list)
                    (if (null? list)
                      intialvalue
                      (eval (+ (* intialvalue x) (car list)) (cdr list))))])
    (eval (car coeffs) (cdr coeffs)))
)

;;Return count of occurrences equal? to x in exp
(define (count-occurrences exp x)
  (if (equal? exp x)
    1
    (if (pair? exp)
      (+ (count-occurrences (car exp) x) (count-occurrences (cdr exp) x))
      0
    )
  )
)

;;Return result of evaluating arith expression over Scheme numbers
;;with fully parenthesized prefix binary operators 'add, 'sub, 'mul
;;and 'div.
(define (eval-arith exp)
  (if (list? exp) 
    (let ([type (car exp)] [op1 (eval-arith (car (cdr exp)))] [op2 (eval-arith (car (cdr (cdr exp))))])
      (cond ((eq? type 'add)
          (+ op1 op2))
        ((eq? type 'sub)
          (- op1 op2))
        ((eq? type 'mul)
          (* op1 op2))
        ((eq? type 'div)
          (/ op1 op2)))
      )
    exp
  )
)
  

;;Given a proper-list list of proper-lists, return sum of lengths of
;;all the contained lists.  Must be tail-recursive.
(define (sum-lengths-tr list)
  (letrec ([tail-sum (lambda (ls intialvalue)
    (if (null? ls)
      intialvalue
      (tail-sum (cdr ls) (+ intialvalue (length (car ls))))
    )
    )])
  (tail-sum list 0)
))

;;Evaluate polynomial with list of coefficients coeffs (highest-order
;;first) at x.  Must be tail-recursive.
(define (poly-eval-tr coeffs x)
  (letrec ([tail-poly (lambda (polyls intialvalue)
    (if (null? polyls)
      intialvalue
      (tail-poly (cdr polyls) (+ intialvalue (* (car polyls) (expt x (- (length polyls) 1)))))
    ))])
    (tail-poly coeffs 0)
  ))

;;Return the list resulting by multiplying each element of `list` by `x`.
;;Cannot use recursion, can use one or more of `map`, `foldl`, or `foldr`.
(define (mul-list-2 list x)
  (map (lambda (ele)
    (* ele x)
  ) list))

;;Given a proper-list list of proper-lists, return the sum of the
;;lengths of all the contained lists.  Cannot use recursion, can use
;;one or more of `map`, `foldl`, or `foldr`.
(define (sum-lengths-2 list)
  (foldl (lambda (ele intialvalue) 
    ( + intialvalue (length ele))
  ) 0 list))
