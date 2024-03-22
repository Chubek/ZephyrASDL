" Vim syntax file
" Language: ASDL
" Author: Chubak Bidpaa

" Clear old syntax items
syntax clear

" Define the main syntax groups
syntax keyword asdlPredefType int uint size usize boolean string identifier contained
syntax match asdlId "\v<[a-z][a-zA-Z0-9_]*>" contained
syntax match asdlConId "\v<[A-Z][a-zA-Z0-9_]*>" contained
syntax match asdlComment "#.*$" contained

" Include C syntax for the embedded C code and ensure it's correctly scoped
syntax region cCode start="%{" end="%}" keepend contains=@C
syntax include @C syntax/c.vim

" Define regions for ASDL structure, ensuring C code is properly contained and doesn't interfere
syntax region asdlPrelude start="^%{" end="%}" keepend contains=cCode
syntax region asdlDefinitions start="^" end="%%" keepend contains=asdlId,asdlConId,asdlPredefType,asdlComment,cCode

" Link the syntax groups to highlight groups
highlight link asdlPredefType Type
highlight link asdlId Identifier
highlight link asdlConId Constant
highlight link asdlComment Commen
