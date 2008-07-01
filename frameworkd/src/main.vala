/* main.vala
 *
 * Copyright (C) 2008  Sudharshan "Sup3rkiddo" S
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:
 * 	Sudharshan "Sup3rkiddo" S <sudharsh@gmail.com>
 */

using GLib;

public class Frameworkd.Main : Object {
	public Main () {
	}

	public void run () {
		stdout.printf ("Hello, world!\n");
	}

	static int main (string[] args) {
		var main = new Main ();
		main.run ();
		return 0;
	}

}
