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

import (
	"serene-lang.org/bootstrap/pkg/ast"
)

// Def defines a global binding in the current namespace. The first
// arguments in `args` has to be a symbol ( none ns qualified ) and
// the second param should be the value of the binding
func Def(rt *Runtime, scope IScope, args *List) (IExpr, IError) {

	// TODO: Add support for docstrings and meta

	switch args.Count() {
	case 2:
		name := args.First()

		if name.GetType() != ast.Symbol {
			return nil, MakeError(rt, "the first argument of 'def' has to be a symbol")
		}

		sym := name.(*Symbol)

		valueExpr := args.Rest().First()
		value, err := EvalForms(rt, scope, valueExpr)

		if err != nil {
			return nil, err
		}

		ns := rt.CurrentNS()
		ns.DefineGlobal(sym.GetName(), value, true)
		return sym, nil
	}

	return nil, MakeError(rt, "'def' form need at least 2 arguments")
}

// Fn defines a function inside the given scope `scope` with the given `args`.
// `args` contains the arugment list, docstring and body of the function.
func Fn(rt *Runtime, scope IScope, args *List) (IExpr, IError) {

	if args.Count() < 1 {
		return nil, MakeError(rt, "'fn' needs at least an arguments list")
	}

	var params IColl
	body := MakeEmptyBlock()

	arguments := args.First()

	// TODO: Add vector in here
	// Or any other icoll
	if arguments.GetType() == ast.List {
		params = arguments.(IColl)
	}

	if args.Count() > 1 {
		body.SetContent(args.Rest().(*List).ToSlice())
	}

	return MakeFunction(scope, params, body), nil
}

// If defines a conditional expression which evaluates the first
// element of the given `args` as the conditional and if the result
// is truthy evaluates and returns the second arg otherwise does the
// same for the third arg.
func If(rt *Runtime, scope IScope, args *List) (IExpr, IError) {

	if args.Count() != 3 {
		return nil, MakeError(rt, "'if' needs exactly 3 aruments")
	}

	pred, err := EvalForms(rt, scope, args.First())
	result := pred.GetType()

	if err != nil {
		return nil, err
	}

	if result != ast.False && result != ast.Nil {
		// Truthy clause
		return EvalForms(rt, scope, args.Rest().First())
	}

	// Falsy clause
	return EvalForms(rt, scope, args.Rest().Rest().First())
}
