module.exports = [
    {
      "type": "heading",
      "defaultValue": "Hybrid"
    },
  
    {
      "type": "section",
      "items": [
        {
          "type": "heading",
          "defaultValue": "Dial colors"
        },
  
        {
        "type": "color",
        "messageKey": "colorBackground",
        "defaultValue": "000000",
        "label": "Background",
        "allowGray": "true",
        },
  
        {
          "type": "color",
          "messageKey": "logoColor",
          "defaultValue": "aaaaaa",
          "label": "Pebble logo",
        },
      ]
    },
  
    {
      "type": "section",
      "items": [
        {
          "type": "heading",
          "defaultValue": "Features"
        },
  
        {
        "type": "toggle",
        "messageKey": "enableSecondHand",
        "defaultValue": "true",
        "label": "Show second hand",
        "description": "Uses more battery"
        },
  
        {
          "type": "toggle",
          "messageKey": "enableVibrateOnDisconnect",
          "defaultValue": "true",
          "label": "Vibrate on disconnect",
          },
      ]
    },
  
    {
      "type": "submit",
      "defaultValue": "Save Settings"
    }
  ];