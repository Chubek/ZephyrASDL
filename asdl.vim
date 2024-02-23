" Vim syntax file
" Language: ASDL
" Author: Chubak Bidpaa

if exists("b:current_syntax")
    finish
endif

syntax include @asdlCCode syntax/c.vim

syntax region asdlPrelude start="%%\n" end="\n%%\n" contains=@asdlCCode,asdlDelimiter

syntax match asdlOperator "="
syntax match asdlOperator ":"
syntax match asdlOperator "|"
syntax match asdlOperator ";"
syntax match asdlOperator ","

syntax match asdlConId /[A-Z][a-zA-Z0-9_]*/
syntax match asdlTypeId /[a-z][a-z0-9_]*/

syntax match asdlDelimiter "%%" contained

highlight link asdlDelimiter Todo
highlight link asdlOperator Operator
highlight link asdlConId Identifier
highlight link asdlTypeId Type

let b:current_syntax = "asdl"

