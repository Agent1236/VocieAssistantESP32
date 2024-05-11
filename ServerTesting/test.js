// -----------------------------------------------------------------------
// EXAMPLE TO OPEN A SERVER AND PRINT HELLO WORLD

// const http = require('node:http');

// const hostname = '127.0.0.1';
// const port = 3000;

// const server = http.createServer((req, res) => {
//   res.statusCode = 200;
//   res.setHeader('Content-Type', 'text/plain');
//   res.end('Hello, World!\n');
// });

// server.listen(port, hostname, () => {
//   console.log(`Server running at http://${hostname}:${port}/`);
// });
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// TEST 1

const express = require('express');
const bodyParser = require('body-parser');
const axios = require('axios');

const app = express();
const port = 3000;

app.use(bodyParser.text());

app.post('/data', async (req, res) => {
    const dataFromESP32 = req.body;
    console.log('Received data from ESP32:', dataFromESP32);
    res.send('Data received successfully');

    sendDataToChatGPT(dataFromESP32);
});

async function sendDataToChatGPT(data) {
    try {
        const payload = {
            model: 'gpt-3.5-turbo-instruct',
            prompt: data,
            temperature: 0.15,
            max_tokens: 40
        };

        const response = await axios.post('https://api.openai.com/v1/completions', payload, {
            headers: {
                'Content-Type': 'application/json',
                'Authorization': 'Bearer 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'
            }
        });

        const textData = response.data.choices[0].text;
        console.log("Response from ChatGPT: ", textData);

        sendTextDataToESP32(textData);
    } catch (error) {
        console.error("Error sending data to ChatGPT API: ", error.message);
    }
}

async function sendTextDataToESP32(textData) {
    try {
        const response = await axios.post('http://192.168.29.157:3000', textData, {
            headers: {
                'Content-Type': 'text/plain'
            }
        });
        console.log('Response from other ESP32:', response.status);
    } catch (error) {
        console.error("Error sending text data to ESP32: ", error.message);
    }
}

app.use((req, res) => {
    res.status(404).send('Not Found');
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});

// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// TEST 2

// const express = require('express');
// const bodyParser = require('body-parser');
// const https = require('https'); // Import the 'https' module

// const app = express();
// const port = 3000;

// app.use(bodyParser.text());

// app.post('/data', async (req, res) => {
//     const dataFromESP32 = req.body;
//     console.log('Received data from ESP32:', dataFromESP32);
//     res.send('Data received successfully');

//     try {
//         const options = {
//             method: 'POST',
//             hostname: 'api.openai.com',
//             path: '/v1/completions',
//             headers: {
//                 'Content-Type': 'application/json',
//                 'Authorization': 'Bearer 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'
//             }
//         };

//         const request = https.request(options, response => {
//             // Handle redirect (status code 301)
//             if (response.statusCode === 301) {
//                 const newLocation = response.headers.location;
//                 console.log(`Redirecting to: ${newLocation}`);
//                 // Make a new request to the new location
//                 https.get(newLocation, newResponse => {
//                     // Handle the response from the new location
//                     let newResponseData = '';
//                     newResponse.on('data', chunk => {
//                         newResponseData += chunk;
//                     });
//                     newResponse.on('end', () => {
//                         console.log("Response from ChatGPT: ", newResponseData);
//                     });
//                 });
//             } else {
//                 // Handle other status codes
//                 let responseData = '';
//                 response.on('data', chunk => {
//                     responseData += chunk;
//                 });
//                 response.on('end', () => {
//                     console.log("Response from ChatGPT: ", responseData);
//                 });
//             }
//         });

//         request.on('error', error => {
//             console.error("Error sending data to ChatGPT API: ", error.message);
//             res.status(500).send("Error processing request");
//         });

//         request.write(JSON.stringify({ prompt: dataFromESP32 }));
//         request.end();
//     } catch (error) {
//         console.error("Error sending data to ChatGPT API: ", error.message);
//         res.status(500).send("Error processing request");
//     }
// });

// app.use((req, res) => {
//     res.status(404).send('Not Found');
// });

// app.listen(port, () => {
//     console.log(`Server running at http://localhost:${port}`);
// });

// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// TEST 3


// const axios = require('axios');
// // set url as constant
// const URL = 'https://jsonplaceholder.typicode.com/todos';

// axios
//     .get(URL)
//     .then(response => {
//         console.log(response);
//     })
//     .catch(error => {
//         console.log(error);
//     });

// -----------------------------------------------------------------------