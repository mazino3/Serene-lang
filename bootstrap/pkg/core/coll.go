/*
 Serene --- Yet an other Lisp

Copyright (c) 2020  Sameer Rahmani <lxsameer@gnu.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

package core

// ISeq is an interface describing a sequence of forms
type ISeq interface {
	First() IExpr
	Rest() ISeq
}

type ICountable interface {
	Count() int
}

// IColl describes a collection of values. A finite collection.
type IColl interface {
	ISeq
	ICountable
	ToSlice() []IExpr
	Cons(e IExpr) IExpr
}
