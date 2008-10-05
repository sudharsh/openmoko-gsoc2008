/* 
 * audio.vala
 * Written by Sudharshan "Sup3rkiddo" S <sudharsh@gmail.com>
 * All Rights Reserved
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */ 

using DBus;
using ODeviced;
using Gst;
using FSO;
using Device;

class GstPlayer : GLib.Object {
	private HashTable<string, string> decoder_table = new HashTable<string, string> ((HashFunc)str_hash, (EqualFunc)str_equal);
	private HashTable<string, Gst.Pipeline> pipelines_table = new HashTable<string, Gst.Pipeline> ((HashFunc)str_hash, (EqualFunc)str_equal);
	
	construct {
		decoder_table.insert ("sid", "siddec");
		decoder_table.insert ("mod", "modplug");
		decoder_table.insert ("mp3", "mad");
	}

	private Gst.Pipeline? createPipeline (string filepath) {
		var _temp = filepath.split (".");

		uint _len = strv_length(_temp);
		string extension = _temp[_len - 1];
		Gst.Pipeline pipeline = new Gst.Pipeline ("name");
		Gst.Element filesrc = Gst.ElementFactory.make("filesrc", "source");
		//GLib.Value value = new GLib.Value(typeof(string));
		//Gst.ChildProxy.set_property (filesrc, "location", value);
		pipeline.add (filesrc);
		string ext = this.decoder_table.lookup(extension);
		if (ext == null) {
			return null;
		}
		
		Gst.Element decoder = Gst.ElementFactory.make (ext, "decoder");
		
		pipeline.add (decoder);
		Gst.Element sink = Gst.ElementFactory.make("alsasink", "sink");
		pipeline.add (sink);
		filesrc.link(decoder);
		decoder.link(sink);
		return pipeline;
	}


	public void task_play (string name, bool repeat) {
		var pipeline = this.createPipeline (name);
		if (pipeline == null)
			return;
		Gst.Bus bus = pipeline.get_bus();
		bus.add_signal_watch();
		/*bus.connect ("message", _onMessage, name);*/
		this.pipelines_table.insert(name, pipeline);
		pipeline.set_state (Gst.State.PLAYING);
	}
				
	public void task_stop (string name) {
		var pipeline = this.pipelines_table.lookup(name);
		if (pipeline == null)
			return;
		pipeline.set_state(Gst.State.READY);
		message ("\t%s Ready", name);
	}
		
}
	
	


[DBus (name = "org.freesmartphone.Device.Audio")]
public class Audio : GLib.Object {

	private GstPlayer player = new GstPlayer();
	
	public void PlaySound (string path) {
		player.task_play (path, false);
	}

	public void StopSound (string path) {
		player.task_stop (path);
	}
}		
	



namespace audio {

	public static Audio obj;

	public bool init (Plugin plugin) {		
		GLib.Thread.init (null);
		obj = new Audio();
		if(obj == null)
			return false;

		ODeviced.register_dbus_object (plugin, obj);
		return true;
	}
}


		
