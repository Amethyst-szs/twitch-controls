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
const eventList = require('../settings/event_list.json').FullEventList;
const spawnList = require('../settings/event_list.json').FullSpawnList;

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
				case "unfreeze":
					outPackets.outHandler('unfreeze', false);
					log.log(3, `Turning off demo lock from Discord by ${interaction.user.username}`);
					interaction.reply(`Turning off demo lock from Discord by ${interaction.user.username}`);
					break;
				case "kill":
					outPackets.outHandler('kill', false);
					log.log(3, `Killing the player from Discord by ${interaction.user.username}`);
					interaction.reply(`Killing the player from Discord by ${interaction.user.username}`);
					break;
				case "preview":
					isPreview = interaction.options.getBoolean("enabled", true);
					previewViewers = interaction.options.getInteger("viewers");

					if(isPreview){
						twitchInit.setPreviewMode(previewViewers);
						log.log(3, `Previewing ${previewViewers} viewers on Twitch`);
						interaction.reply(`Check the Twitch account to preview ${previewViewers} viewers!`);
					} else {
						twitchInit.setPreviewMode(-1);
						log.log(3, `Disabling preview mode on Twitch`);
						interaction.reply(`Preview mode has been disabled!`);
					}
					
					twitchInit.skipRefreshTimer();
					break;
				case "do":
					log.debugLog(`Discord event trigger (event)`);
					outPackets.outAnyEventHandler(interaction.options.getString("event", true), "Event");
					interaction.reply(`Sending event *"${interaction.options.getString("event", true)}"*`);
					break;
				case "spawn":
					log.debugLog(`Discord event trigger (spawn)`);
					outPackets.outAnyEventHandler(interaction.options.getString("spawn", true), "Spawn");
					interaction.reply(`Sending spawn *"${interaction.options.getString("spawn", true)}"*`);
					break;
				case "boot-off":
					outPackets.pushNewBlock(log.getNickname());
					outPackets.blockPacket(outPackets.getClient());
					log.log(3, `Kicking ${log.getNickname()} off of the server!`);
					interaction.reply(`Kicked the current client off!`);
					break;
				case "music":
					outPackets.toggleMusic();
					interaction.reply(`Music toggled!`);
					break;
				// case "flee-killswitch":
				// 	outPackets.fleeKillswitch();
				// 	interaction.reply(`Toggled the kingdom flee feature!`);
				// 	break;
				case "scost":
					redeemName = interaction.options.getString("redeem", true);
					factor = interaction.options.getNumber("factor", true);

					twitchInit.sCostUpdate(redeemName, factor);
					log.log(3, `Updating cost factor of ${redeemName} to ${factor}`);
					interaction.reply(`Updating cost factor of ${redeemName} to ${factor}`);
					break;
				case "scooldown":
					redeemName = interaction.options.getString("redeem", true);
					factor = interaction.options.getNumber("factor", true);

					twitchInit.sCooldownUpdate(redeemName, factor);
					log.log(3, `Updating cooldown factor of ${redeemName} to ${factor}`);
					interaction.reply(`Updating cooldown factor of ${redeemName} to ${factor}`);
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
		let formattedEvents = [];
		let formattedSpawns = [];

		//Format this list into a weird formatted redeems array for the twitch API
		for(i=0;i<FullRedeemList.length;i++){formattedRedeems.push([FullRedeemList[i], FullRedeemList[i]])}
		for(i=0;i<eventList.length;i++){formattedEvents.push([eventList[i], eventList[i]])}
		for(i=0;i<spawnList.length;i++){formattedSpawns.push([spawnList[i], spawnList[i]])}

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
			new SlashCommandBuilder()
				.setName('unfreeze')
				.setDescription('Turns off the players demo state, saving from potential softlocks'),
			new SlashCommandBuilder()
				.setName('kill')
				.setDescription('Kills Mario, mainly use this to save a player from softlocks'),
			new SlashCommandBuilder()
				.setName('preview')
				.setDescription('Preview your current prices with different viewer counts')
				.addBooleanOption(option => 
					option.setName("enabled")
					.setDescription("Is preview mode on?")
					.setRequired(true)
				)
				.addIntegerOption(option =>
					option.setName('viewers')
					.setDescription('How many viewers are we testing?')
					.setRequired(true)
				),
			new SlashCommandBuilder()
				.setName('do')
				.setDescription('Sends any event to the client')
				.addStringOption(option =>
					option.setName('event')
					.setDescription('Which event are you sending')
					.setRequired(true)
					.addChoices(formattedEvents)
				),
			new SlashCommandBuilder()
				.setName('spawn')
				.setDescription('Sends any entity spawn to the client')
				.addStringOption(option =>
					option.setName('spawn')
					.setDescription('Which spawn are you sending')
					.setRequired(true)
					.addChoices(formattedSpawns)
				),
			// new SlashCommandBuilder()
			// 	.setName('fake-id')
			// 	.setDescription('Sends a fake redeem id')
			// 	.addNumberOption(option =>
			// 		option.setName('id')
			// 		.setDescription('What is the id')
			// 		.setRequired(true),
			new SlashCommandBuilder()
				.setName('boot-off')
				.setDescription('Kicks the current client off, blocking them for this session'),
			new SlashCommandBuilder()
				.setName('music')
				.setDescription('Toggles the game music, defaults off'),
			// new SlashCommandBuilder()
			// 	.setName('flee-killswitch')
			// 	.setDescription('Turns off the experimental kingdom flee feature'),
			new SlashCommandBuilder()
				.setName('scost')
				.setDescription('Set a price multiplier on one specific redeem')
				.addStringOption(option =>
					option.setName('redeem')
					.setDescription('What redeem are you changing?')
					.setRequired(true)
					.addChoices(formattedRedeems)
				)
				.addNumberOption(option =>
					option.setName('factor')
						.setDescription('What is the multi cost factor of this redeem?')
						.setRequired(true)	
				),
			new SlashCommandBuilder()
				.setName('scooldown')
				.setDescription('Set a cooldown multiplier on one specific redeem')
				.addStringOption(option =>
					option.setName('redeem')
					.setDescription('What redeem are you changing?')
					.setRequired(true)
					.addChoices(formattedRedeems)
				)
				.addNumberOption(option =>
					option.setName('factor')
						.setDescription('What is the multi cooldown factor of this redeem?')
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