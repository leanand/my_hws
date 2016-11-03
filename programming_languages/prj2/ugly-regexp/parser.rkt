#!/usr/bin/env racket
;;-*- mode: scheme; -*-
;; :set filetype=scheme

#lang racket

(require "errors.rkt")
(require "scanner.rkt")

;;;************************* Top-Level Globals *************************

;;global var which contains lookahead token
(define *lookahead* #f)

;;global var which specifies function which reads next token from scanner
(define *next-token* #f)

;;;****************************** Parser *******************************

;;Top-level parse function.
;;Parse ugly regexp from line and output translation in standard syntax.
;;If error, catch syntax-error exception, print message and recover.
(define (parse)
  (unless (equal? '<EOF> (token-kind *lookahead*))
	  (with-handlers
	    ([exn:fail:user?
	      (lambda (ex)
		(do-error (exn-message ex))
		(recover-from-error)
		(advance))])
	    (let ([out (ugly-regexp)])
	      (when (check? '<NL>) (displayln out))
	      (match '<NL>)))
	  (parse)))

(define (ugly-regexp)
  (let ([t (term)])
    (regexp-rest t)))

(define (regexp-rest t)
  (if (check? 'CHAR ".")
      (begin
	(match 'CHAR ".")
	(let ([t1 (term)])
	  (regexp-rest (string-append "(" t t1 ")"))))
      t))

;;Flesh out skeleton functions below.
(define (term)
  (let ([f (factor)])
    (term-rest f)))

(define (term-rest f)
  (if (check? 'CHAR "+")
      (begin
  (match 'CHAR "+")
  (let ([f1 (factor)])
    (term-rest (string-append "(" f "|" f1 ")"))))
      f))

(define (factor)
  (cond 
    ((check? 'CHAR "(") 
      (begin 
        (match 'CHAR "(")
        (let ([ugly (ugly-regexp)])
          (match 'CHAR ")")
          (string-append "(" ugly ")"))))
    ((check? 'CHAR "*")
      (begin 
        (match 'CHAR "*")
        (let ([f (factor)])
          (string-append f "*"))))
    (else 
      (begin
        (match 'CHARS)
        (match 'CHAR "(")
        (let ([c (chars)])
          (match 'CHAR ")")
          (string-append "[" c "]"))))
  ))

(define (chars)
  (let ([str (quote-char(token-lexeme *lookahead*))])
    (begin
      (match 'CHAR)
      (if (check? 'CHAR ",")
        (begin 
          (match 'CHAR ",")
          (let ([str1 (chars)])
            (string-append str str1)
          )
        )
        str)
    ))
)

;;;********************** Parser Utility Functions *********************

;;invoke as either (match KIND) or (match KIND LEXEME). If 
;;lookahead.kind == KIND and lookahead.lexeme == LEXEME (when LEXEME
;;is specified), then advances lookahead; else throws an exception.
(define (match . args)
  (if (apply check? args)
      (advance)
      (raise-user-error (make-error-message args))))

;;invoke as either (check? KIND) or (check? KIND LEXEME).  Returns true
;;iff lookahead.kind == KIND and lookahead.lexeme == LEXEME (when LEXEME
;;is specified).
(define (check? kind . rest)
  (and (equal? kind (token-kind *lookahead*))
       (or (equal? (length rest) 0)
	   (equal? (car rest) (token-lexeme *lookahead*)))))

;;quote first char of string if not word-char or digit.
(define (quote-char str)
  (if (regexp-match #px"^[\\w\\d]" str)
      str
      (string-append "\\" str)))

(define (advance) (set! *lookahead* (*next-token*)))

;;;************************* Error Handling ****************************

;;create error message for mismatch between *lookahead* and args
;;which is either (KIND) or (KIND LEXEME)
(define (make-error-message args)
  (let* ([kind (car args)]
	 [expected (if (equal? (length args) 2) (cadr args) kind)])
    (format "~a: syntax error at '~a', expecting '~s'"
	    (position->string (token-position *lookahead*))
	    (token-lexeme *lookahead*)
	    expected)))

(define (recover-from-error)
  (unless (or (check? '<NL>) (check? '<EOF>))
	  (advance)
	  (recover-from-error)))

;;;*************************** Main Program ****************************

(define (run-parse argv)
  (if (not (equal? (vector-length argv) 1))
      (usage)
      (begin
	(set! *next-token* (scanner (vector-ref argv 0)))
	(advance)    ;prime lookahead
	(parse))))
       
(run-parse (current-command-line-arguments))
				
				

