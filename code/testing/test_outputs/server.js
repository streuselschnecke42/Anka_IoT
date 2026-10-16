const express = require("express");
const app = express();

app.use(express.json());

app.post("/logger", (req, res) => {

    console.log("NEW EVENT REVCEIVED");
    console.log(req.body);

    res.sendStatus(200);
});

app.listen(2006, () => {
    console.log("Web server running on port 2006");
});

