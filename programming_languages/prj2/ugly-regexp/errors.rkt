#lang racket

(provide do-error usage)

(define (do-error . args)
  (apply eprintf args)
  (eprintf "~n"))

(define (usage)
  (do-error "usage: ~a FILENAME|-" (find-system-path 'run-file))
  (exit 1))

