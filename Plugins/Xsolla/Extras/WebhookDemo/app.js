// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

const config = require('./config.json');
global.gConfig = config;

// Prepare express and logger extensions
let express = require('express');
let logger = require('morgan');

// Create in-memory only datastore
let Datastore = require('nedb');
global.db = {};
global.db.users = new Datastore();
global.db.purchases = new Datastore();

// And fill the db with test data
global.db.users.insert([
    { id: "ladora@directmail.top" },
    { id: "d342dad2-9d59-11e9-a384-42010aa8003f" },		// "name": "xsolla"
    { id: "test_user_1", email: "test@test.ru", password: "111" },
    { id: "xsolla", email: "ufna@ufna.ru", password: "xsolla" },
    { id: "test_user_2" }],
    function (err, newDocs) {
    console.log("Demo users were inserted in the database");
});

// Prepare router and launch the app
let webhookRouter = require('./routes/webhook');
let tokenRouter = require('./routes/verify');
let proxyVerifyUserRouter = require('./routes/userVerify');
let proxyRegisterUserRouter = require('./routes/userRegister');

let app = express();

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));

app.use('/webhook', webhookRouter);
app.use('/verify', tokenRouter);
app.use('/userVerify', proxyVerifyUserRouter);
app.use('/userRegister', proxyRegisterUserRouter);

module.exports = app;
