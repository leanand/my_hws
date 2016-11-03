#!/usr/bin/env racket
;;-*- mode: scheme; -*-
;; :set filetype=scheme

#lang racket

(require "errors.rkt")

;;;***************************** Positions *****************************

(provide position->string)

(define (position filename lineN colN)
  (let ([name (if (equal? filename "-") "<stdin>" filename)])
    (list name lineN colN)))
(define (position-filename pos) (car pos))
(define (position-linenumber pos) (cadr pos))
(define (position-colnumber pos) (caddr pos))
(define (position->string pos)
  (format "~a:~a:~a" (position-filename pos) 
	  (position-linenumber pos) (position-colnumber pos)))

;;if (len >= 0), update col # by len; else increment line #.
(define (position-update pos len)
  (if (< len 0)
      (position (position-filename pos)
		(+ 1 (position-linenumber pos))
		0)
      (position (position-filename pos)
		(position-linenumber pos)
		(+ (position-colnumber pos) len))))

;;;****************************** Tokens *******************************

(provide token-kind token-lexeme token-position token->string)

(define (token type lexeme pos) (list type lexeme pos))
(define (token-kind token) (car token))
(define (token-lexeme token) (cadr token))
(define (token-position token) (caddr token))
(define (token->string tok)
  (format "[~a, ~a, ~a]" (token-kind tok) (token-lexeme tok)
	  (position->string (token-position tok))))

;;;************************* Scanner Routines **********************

(provide scanner)

(define *CHARS* "chars")

;;Returns a next-token function which takes no arguments.  Successive
;;calls return successive tokens from filename with token-kind set
;;to '<EOF> for the last token.
(define (scanner filename)
  (let* ([in (get-scanner-port filename)]
	 [lines (line-stream in)]
	 [pos (position filename 1 0)]
	 [dummy-tok (token 'DUMMY "<DUMMY>" pos)]
	 [state (scan-state dummy-tok pos '() lines)]
         [token-stream (scan-stream (scan-state-next state))])
    (lambda ()
      (let ([token (stream-first token-stream)])
	(set! token-stream (stream-rest token-stream))
	token))))

(define (scan-state tok pos line lines) (list tok pos line lines))
(define (scan-state-last-token state) (car state))
(define (scan-state-position state) (cadr state))
(define (scan-state-line state) (caddr state))
(define (scan-state-lines state) (cadddr state))
(define (scan-state->string state)
  (format "<~a, ~a, ~a>" (token->string (scan-state-last-token state)) 
		 (position->string (scan-state-position state)) 
		 (scan-state-line state)))

(define (scan-state-next state)
  (cond
   [(null? (scan-state-line state))
    (scan-state-next
     (scan-state (scan-state-last-token state) (scan-state-position state)
		 (stream-first (scan-state-lines state))
		 (stream-rest (scan-state-lines state))))]
   [(equal? eof (scan-state-line state))
    (scan-state (token '<EOF> "<EOF>" (scan-state-position state))
		(scan-state-position state) '() '())]
   [else
    (let* ([line (scan-state-line state)]
	     [lines (scan-state-lines state)]
	     [trimmed (string-trim line)]
	     [trimmed-len (- (string-length line) (string-length trimmed))]
	     [pos (position-update (scan-state-position state) trimmed-len)])
	(cond
	 [(string=? "" trimmed)
	    (scan-state (token '<NL> "<NL>" pos) (position-update pos -1)
			'() lines)]
	 [(string-prefix? trimmed *CHARS*)
	  (scan-state (token 'CHARS *CHARS* pos)
		      (position-update pos (string-length *CHARS*))
		      (substring trimmed (string-length *CHARS*)) lines)]
	 [else (scan-state (token 'CHAR (substring trimmed 0 1) pos)
			   (position-update pos 1)
			   (substring trimmed 1) lines)]))]))
  

(define (line-stream in)
  (let ((line (read-line in)))
    (if (equal? line eof)
	(stream-cons line empty-stream)
	(stream-cons line (line-stream in)))))

(define (scan-stream state)
  (stream-cons (scan-state-last-token state)
	       (if (equal? '<EOF> (token-kind (scan-state-last-token state)))
		   empty-stream
		   (scan-stream (scan-state-next state)))))

(define (get-scanner-port filename)
  (if (equal? filename "-")
      (current-input-port)
      (open-input-file filename)))

;;;*************************** Utility Routines **********************

;;available as standard in racket v6.3.
(define (string-prefix? s prefix)
  (let ([prefix-len (string-length prefix)])
    (and (>= (string-length s) prefix-len)
	 (equal? (substring s 0 prefix-len) prefix))))

;;;***************************** Test Program ************************

(define (run-scan argv)
  (if (not (equal? (vector-length argv) 1))
      (usage)
      (letrec ([next-token (scanner (vector-ref argv 0))]
	       [out-token
		(lambda (tok)
		  (display (token->string tok)) (display "\n")
		  (unless (equal? '<EOF> (token-kind tok))
			  (out-token (next-token))))])
	(out-token (next-token)))))        

(let-values
    ([(_1 __FILE__ _2) (split-path (syntax-source #'here))]
     [(_3 run-file _4) (split-path (find-system-path 'run-file))])
  (when (equal? __FILE__ run-file)
      (run-scan (current-command-line-arguments))))

