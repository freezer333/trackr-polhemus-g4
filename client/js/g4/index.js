var net = require('net');

var g4 = {

	initialize : function(callback, host, port) {
		var hostname = host || "localhost";
		var port_number = port || 1986;

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
					pos : {
						x : fields[1],
						y : fields[2],
						z : fields[3],
					}, 
					ori : {
						x : fields[4], 
						y : fields[5], 
						z : fields[6], 
						w : fields[7]
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
		req = new Buffer([true, true, false]);
		this.socket.write(req);
	}

}

if (typeof module !== 'undefined') {
  module.exports = g4;
}