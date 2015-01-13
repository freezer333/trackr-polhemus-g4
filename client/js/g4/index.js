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
				var po = {
					frame : fields[0], 
					sensor : fields[1],
					pos : {
						x : fields[2],
						y : fields[3],
						z : fields[4],
					}, 
					ori : {
						x : fields[5], 
						y : fields[6], 
						z : fields[7], 
						w : fields[8]
					}
				}
				callback(null, po);
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