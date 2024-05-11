const express = require('express');
const bodyParser = require('body-parser');
const axios = require('axios');

const app = express();
const port = SERVER_PORT;

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
                'Authorization': CHATGPT_API_KEY
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
        const response = await axios.post(SERVER_ADDRESS_WITH_PORT, textData, {
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
