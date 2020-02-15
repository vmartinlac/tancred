const express = require('express')
const app = express()
const fs = require('fs')

PORT = 8080
ARDUINO_FILENAME = '/dev/ttyACM0'

arduino = fs.openSync(ARDUINO_FILENAME, 'w')

app.set('view engine', 'pug')

app.use(express.static('public'))

app.post('/direction-left', (req, res) => {
    fs.writeSync(arduino, 'direction-left ')
    res.send('OK')
})

app.post('/direction-right', (req, res) => {
    fs.writeSync(arduino, 'direction-right ')
    res.send('OK')
})

app.post('/direction-front', (req, res) => {
    fs.writeSync(arduino, 'direction-front ')
    res.send('OK')
})

app.post('/speed-forward', (req, res) => {
    fs.writeSync(arduino, 'speed-forward ')
    res.send('OK')
})

app.post('/speed-backward', (req, res) => {
    fs.writeSync(arduino, 'speed-backward ')
    res.send('OK')
})

app.post('/speed-stop', (req, res) => {
    fs.writeSync(arduino, 'speed-stop ')
    res.send('OK')
})

app.get('/', (req, res) => {
    res.render('index')
})

app.listen(PORT)

