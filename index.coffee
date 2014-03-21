_ = require 'underscore'
Leap = require 'leapjs'

controller = new Leap.Controller(enableGestures: true)
# mode = 'http'
mode = 'serial'
debug = false
switch mode
    when 'serial'
        dev = "/dev/tty.usbmodem1421"
        open = false
        SerialPort = require("serialport").SerialPort
        serialPort = new SerialPort dev,
            baudrate: 9600
        serialPort.on 'open', (e) ->
            open = true

    when 'http'
        shred = new require('shred')();
        yun_endpoint = 'arduino/hand'
        yun_host = 'yuny.local'

process_debounced = _.throttle((frame) ->
    if frame.hands.length
        hand = frame.hands[0]
        handz = hand.palmPosition[1]
        if debug
            console.log handz, hand.pitch(), hand.roll()

        switch mode
            when 'serial'
                break if not open
                # converting and sending as string, figure out how to pass integers
                out = "##{parseInt(handz)},#{hand.pitch().toFixed(4)},#{hand.roll().toFixed(4)}"
                console.log out
                serialPort.write new Buffer(out), (err, results) ->
                    return if not debug
                    console.log('err ' + err) if err
                    console.log('results ' + results) if results
            when 'http'
                try
                    # eg: http://yuny.local/arduino/hand/300/3/2
                    full_url = "http://#{yun_host}/#{yun_endpoint}/#{handz}/#{hand.pitch()}/#{hand.roll()}"
                    shred.get
                        url: full_url
                catch e
                    console.error(e)

    # detect cw and cww circle gestures
    # if frame.gestures.length
    #   for gesture in frame.gestures
    #       if gesture.type is 'circle'
    #           gesture.pointable = frame.pointable(gesture.pointableIds[0])
    #           dir = gesture.pointable.direction
    #           console.log if Leap.vec3.dot(dir, gesture.normal) > 0 then 'cw' else 'ccw'
    return frame
, 30)

controller.on 'frame', (frame) ->
    return if not frame.valid
    process_debounced(frame)

controller.on "ready", ->
  console.log "ready"

# controller.on "connect", ->
#   console.log "connect"

# controller.on "disconnect", ->
#   console.log "disconnect"

# controller.on "focus", ->
#   console.log "focus"

# controller.on "blur", ->
#   console.log "blur"

# controller.on "deviceConnected", ->
#   console.log "deviceConnected"

# controller.on "deviceDisconnected", ->
#   console.log "deviceDisconnected"

controller.connect()
console.log "\nWaiting for device to connect..."

process.on('uncaughtException', (e) ->
    console.error e
)
