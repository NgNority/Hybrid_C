module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Here is some introductory text."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "BackgroundColor",
        "defaultValue": "0x000000",
        "label": "Background Color",
        "allowGray": true
      },
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "More Settings"
      },
      {
        "type": "toggle",
        "messageKey": "batteryBar",
        "label": "Enable battery bar",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "weatherDisplay",
        "label": "Enable weather",
        "defaultValue": true
      },
      {
        "type": "slider",
        "messageKey": "handLength",
        "defaultValue": 15,
        "label": "Hand Length",
        "description": "Use this slider to adjust the length of the hands",
        "min": 1,
        "max": 10,
        "step": 1
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];