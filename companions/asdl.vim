" Vim syntax file
" Language: ASDL
" Author: Chubak Bidpaa

if exists("b:current_syntax")
    finish
endif

syntax include @asdlCCode syntax/c.vim

syntax region asdlPrelude start="%{" end="%}" contains=@asdlCCode
syntax region asdlAppend start="\n%%\n" end="\%$" contains=@asdlCCode,asdlPcntPcnt

syntax match asdlOperator "="
syntax match asdlOperator ":"
syntax match asdlOperator "|"
syntax match asdlOperator ";"
syntax match asdlOperator ","

syntax match asdlConId /\v[A-Z][a-zA-Z0-9_]*/
syntax match asdlTypeId /\v[a-z][a-z0-9_]*/

syntax match asdlPcntPctn "%%" contained

syntax keyword asdlKeywords bool int uint size usize string identifier

highlight link asdlOperator Operator
highlight link asdlConId Identifier
highlight link asdlTypeId Type
highlight link asdlKeywords Keyword
highlight link asdlPcntPcnt Todo

let b:current_syntax = "asdl"

