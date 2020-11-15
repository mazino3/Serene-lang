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
package cmd

import (
	"github.com/spf13/cobra"
	"serene-lang.org/bootstrap/pkg/core"
)

// replCmd represents the base command when called without any subcommands
var replCmd = &cobra.Command{
	Use:   "repl",
	Short: "Runs the local Serene's REPL",
	Long:  `Runs the local Serene's REPL to interact with Serene`,
	Run: func(cmd *cobra.Command, args []string) {
		// TODO: Get the debug value from a CLI flag
		core.REPL(debugMode)
	},
}

func init() {
	rootCmd.AddCommand(replCmd)
}