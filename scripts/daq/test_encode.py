import cantools

db = cantools.database.load_file("dbc/veh.dbc")

msg = db.get_message_by_name("DashCommand")

data = msg.encode({
	"Speed": 30,
	"HvSocPercent": 50,
	"ConfigReceived": 1,
	"HvStarted": 1,
	"MotorStarted": 1,
	"DriveStarted": 1,
	"Errored": 0,
	"HvPrechargePercent": 50
})

print(data.hex())


