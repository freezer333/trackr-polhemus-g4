var net = require('net');

var g4 = {

	initialize : function(callback, sensor_opt_in, host, port) {
		var hostname = host || "localhost";
		var port_number = port || 1986;
		this.sensor_opt_in = sensor_opt_in || [true, true, true];
		this.callback = callback;
		this.socket = new net.Socket();
		this.socket.connect(port_number, hostname, function(err) {
			if ( err ) {
				console.log("Error connecting to g4 tracker")
			}
			else {
				console.log('CONNECTED TO: ' + hostname + ':' + port_number);
			}
		});

		this.socket.on('error', function(err) {
			console.log(err);
		});

		this.socket.on('data', function(data) {
			try {
				var fields = data.toString().split("|");
				var po_array = [];
				for ( var i = 0; i < fields.length; i+= 8) {
					var po = {
						frame : fields[i + 0], 
						sensor : fields[i + 1],
						pos : {
							x : fields[i + 2],
							y : fields[i + 3],
							z : fields[i + 4],
						}, 
						ori : {
							x : fields[i + 5], 
							y : fields[i + 6], 
							z : fields[i + 7], 
						}
					}
					po_array.push(po);
				}
				callback(null, po_array);
			}
			catch (err) {
				console.log(err);
			}    
		});
	}, 

	poll  : function (callback) {
		req = new Buffer(this.sensor_opt_in);
		this.socket.write(req);
	}

}

if (typeof module !== 'undefined') {
  module.exports = g4;
}