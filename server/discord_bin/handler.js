// Require the necessary discord.js classes
const { Client, Intents } = require('discord.js');
const { SlashCommandBuilder } = require('@discordjs/builders');
const { REST } = require('@discordjs/rest');
const { Routes } = require('discord-api-types/v9');
const fs = require('fs');
const { disclientid, disguildid, distoken } = require('../settings/secret.json');

//Require other modules
const log = require('../server_bin/console');
const twitchInit = require('../server_bin/twitchInit');
const restrictions = require('../server_bin/restrictions');
const outPackets = require('../server_bin/outPackets');

// Create a new client instance
const client = new Client({ intents: [Intents.FLAGS.GUILDS] });

//Global variables
let costFact = undefined;
let cooldownMulti = 1;

function writeStatus(){
	client.user.setActivity(`TC - Cost Factor: ${costFact} Cooldown Factor: ${cooldownMulti}`);
}

module.exports = {
	botManage: function(){
		// When the client is ready, run this code (only once)
		client.once('ready', () => {
			log.log(3, "Initalized Discord Bot for managing the server!");
			client.user.setActivity(`Twitch Controls - Default Settings`);
		});

		client.on('interactionCreate', async interaction => {
			if (!interaction.isCommand()) return;
			interaction.options.getString()
			const { commandName } = interaction;
			
			switch(commandName){
				case "ping":
					log.log(3, "Pong!");
					await interaction.reply('Pong!');
					break;
				case "cost":
					twitchInit.disCostUpdate(interaction.options.getString('modification', true), interaction.options.getNumber('value', true));
					log.log(3, `Pushing a cost update from ${interaction.user.username}`);
					interaction.reply(`Performing **${interaction.options.getString('modification', true)}** operation on cost!!`);
					costFact = interaction.options.getNumber('value', false);
					writeStatus();
					break;
				case "cooldown":
					value = interaction.options.getNumber('value', true);
					twitchInit.disCooldownUpdate(value);
					log.log(3, `Pushing a cooldown update from ${interaction.user.username}`);
					interaction.reply(`Setting the cooldown multiplier to **${value}**!`);
					cooldownMulti = value;
					writeStatus();
					break;
				case "enabled":
					value = interaction.options.getString('redeem', true);
					restrictions.updateForcedRestriction(value);
					twitchInit.skipRefreshTimer();
					log.log(3, `Toggling a forced restriction from ${interaction.user.username}`);
					interaction.reply(`Toggling the state of **${value}** for you!`);
					break;
				case "toggle-all":
					restrictions.toggleAllDisabled();
					twitchInit.skipRefreshTimer();
					log.log(3, `Toggling all redeems from ${interaction.user.username}`);
					interaction.reply(`Toggling every single Twitch Controls redeem for you!`);
					break;
				case "say":
					message = interaction.options.getString('message', true);

					//Error check
					if(message.length > 450){
						log.log(3, `Message is ${message.length} characters long, valid range is 2-450!`);
						interaction.reply(`Your message is ${message.length} characters long, get it within 2-450 characters!`);
						return;
					};

					log.log(3, `Sending ${message} from ${interaction.user.username} to client`);
					interaction.reply(`Sending your message off to the client!\n${message}`);
					outPackets.sayMessage(message);
					break;
				default:
					interaction.reply("**ERROR**\nDunno what you did, but it didn't work");
					log.log(3, "Strange interaction, ignoring");
					break;
			}
		});

		// Login to Discord with your client's token
		client.login(distoken);
	},
	
	slashCommandInit: function(CurDir){
		//Get a list of the current redeems
		const FullRedeemList = JSON.parse(fs.readFileSync(`${CurDir}/settings/localize/${twitchInit.getLang()}_list.json`)).FullRedeemList;
		let formattedRedeems = [];

		//Format this list into a weird formatted redeems array for the twitch API
		for(i=0;i<FullRedeemList.length;i++){
			formattedRedeems.push([
				FullRedeemList[i],
				FullRedeemList[i]
			])
		}

		const commands = [
			new SlashCommandBuilder()
				.setName('ping')
				.setDescription('Verifies the server is still running by sending back a pong'),
			new SlashCommandBuilder()
				.setName('cost')
				.setDescription('Update the channel point pricing on the fly')
				.addStringOption(option =>
					option.setName('modification')
					.setDescription('How do you want to change your pricing')
					.setRequired(true)
					.addChoice('Increase', 'inc')
					.addChoice('Decrease', 'dec')
					.addChoice('Set', 'set')
					.addChoice('Toggle Debug Pricing', 'dis')
				)
				.addNumberOption(option =>
					option.setName('value')
						.setDescription('How much to increase or decrease price multiplier OR set it to')
						.setRequired(true)
				),
			new SlashCommandBuilder()
				.setName('cooldown')
				.setDescription('Set a multiplier on redeem cooldowns!')
				.addNumberOption(option =>
					option.setName('value')
						.setDescription('What should the multiplier for cooldowns be?')
						.setRequired(true)
				),
			new SlashCommandBuilder()
				.setName('enabled')
				.setDescription('Toggles on and off specific redeems')
				.addStringOption(option =>
					option.setName('redeem')
					.setDescription('Which redeem are you toggling')
					.setRequired(true)
					.addChoices(formattedRedeems)
				),
			new SlashCommandBuilder()
				.setName('toggle-all')
				.setDescription('Turns on or off every single Twitch Controls redeem on your channel'),
			new SlashCommandBuilder()
				.setName('say')
				.setDescription('Send a message to the screen of the person playing')
				.addStringOption(option =>
					option.setName('message')
					.setDescription('What are you saying')
					.setRequired(true)
				),
		]
			.map(command => command.toJSON());
		
		const rest = new REST({ version: '9' }).setToken(distoken);
		
		rest.put(Routes.applicationGuildCommands(disclientid, disguildid), { body: commands })
			.then(() => log.log(3, 'Successfully registered slash commands'))
			.catch(console.error);
	}
}